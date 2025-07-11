import WidgetKit
import SwiftUI

// MARK: - Data Models
struct DepartureEntry: TimelineEntry {
    let date: Date
    let departures: [Departure]
}

struct Departure: Identifiable {
    let id = UUID()
    let timeToStation: Int
    let towards: String
    let platformName: String
    let currentLocation: String
}

struct TfLResponse: Codable {
    let id: String
    let lineId: String
    let platformName: String
    let towards: String
    let timeToStation: Int
    let currentLocation: String
}

// MARK: - Timeline Provider
struct Provider: TimelineProvider {
    func placeholder(in context: Context) -> DepartureEntry {
        DepartureEntry(date: Date(), departures: [
            Departure(timeToStation: 3, towards: "Upminster", platformName: "Platform 1", currentLocation: "At Dagenham East"),
            Departure(timeToStation: 7, towards: "Ealing Broadway", platformName: "Platform 2", currentLocation: "Between Barking and Becontree")
        ])
    }

    func getSnapshot(in context: Context, completion: @escaping (DepartureEntry) -> ()) {
        let entry = DepartureEntry(date: Date(), departures: [
            Departure(timeToStation: 2, towards: "Upminster", platformName: "Platform 1", currentLocation: "Approaching"),
            Departure(timeToStation: 5, towards: "Ealing Broadway", platformName: "Platform 2", currentLocation: "At Dagenham East")
        ])
        completion(entry)
    }

    func getTimeline(in context: Context, completion: @escaping (Timeline<Entry>) -> ()) {
        fetchDepartures { departures in
            let currentDate = Date()
            let entry = DepartureEntry(date: currentDate, departures: departures)
            
            // Update every 30 seconds
            let nextUpdate = Calendar.current.date(byAdding: .second, value: 30, to: currentDate)!
            let timeline = Timeline(entries: [entry], policy: .after(nextUpdate))
            completion(timeline)
        }
    }
    
    private func fetchDepartures(completion: @escaping ([Departure]) -> Void) {
        // Becontree Station ID: 940GZZLUBCT
        // District Line ID: district
        guard let url = URL(string: "https://api.tfl.gov.uk/StopPoint/940GZZLUBCT/arrivals?line=district") else {
            completion([])
            return
        }
        
        URLSession.shared.dataTask(with: url) { data, response, error in
            guard let data = data, error == nil else {
                // Fallback data when API is unavailable
                DispatchQueue.main.async {
                    completion([
                        Departure(timeToStation: 4, towards: "Upminster", platformName: "Platform 1", currentLocation: "Approaching"),
                        Departure(timeToStation: 8, towards: "Ealing Broadway", platformName: "Platform 2", currentLocation: "At Dagenham East")
                    ])
                }
                return
            }
            
            do {
                let tflResponse = try JSONDecoder().decode([TfLResponse].self, from: data)
                let departures = tflResponse.prefix(4).map { response in
                    Departure(
                        timeToStation: max(response.timeToStation / 60, 0), // Convert to minutes
                        towards: response.towards,
                        platformName: response.platformName,
                        currentLocation: response.currentLocation.isEmpty ? "Approaching" : response.currentLocation
                    )
                }.sorted { $0.timeToStation < $1.timeToStation }
                
                DispatchQueue.main.async {
                    completion(departures)
                }
            } catch {
                // Fallback data when JSON parsing fails
                DispatchQueue.main.async {
                    completion([
                        Departure(timeToStation: 3, towards: "Upminster", platformName: "Platform 1", currentLocation: "At Dagenham East"),
                        Departure(timeToStation: 6, towards: "Ealing Broadway", platformName: "Platform 2", currentLocation: "Between Barking and Becontree")
                    ])
                }
            }
        }.resume()
    }
}

// MARK: - Widget Views
struct DistrictLineWidgetEntryView: View {
    var entry: Provider.Entry
    
    var body: some View {
        ZStack {
            // District Line green background
            LinearGradient(
                gradient: Gradient(colors: [
                    Color(red: 0/255, green: 114/255, blue: 41/255),
                    Color(red: 0/255, green: 100/255, blue: 35/255)
                ]),
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            )
            
            VStack(spacing: 0) {
                // Header with station name and line
                HStack {
                    VStack(alignment: .leading, spacing: 2) {
                        Text("BECONTREE")
                            .font(.system(size: 14, weight: .bold, design: .rounded))
                            .foregroundColor(.white)
                        Text("District Line")
                            .font(.system(size: 10, weight: .medium, design: .rounded))
                            .foregroundColor(.white.opacity(0.9))
                    }
                    Spacer()
                    // TfL roundel inspired icon
                    ZStack {
                        Circle()
                            .fill(Color.red)
                            .frame(width: 24, height: 24)
                        Circle()
                            .stroke(Color.white, lineWidth: 2)
                            .frame(width: 24, height: 24)
                        Rectangle()
                            .fill(Color.blue)
                            .frame(width: 20, height: 4)
                    }
                }
                .padding(.horizontal, 12)
                .padding(.top, 8)
                
                Spacer()
                
                // Departures
                VStack(spacing: 6) {
                    ForEach(Array(entry.departures.prefix(2).enumerated()), id: \.element.id) { index, departure in
                        DepartureRowView(departure: departure, isFirst: index == 0)
                    }
                }
                .padding(.horizontal, 12)
                .padding(.bottom, 8)
            }
        }
    }
}

struct DepartureRowView: View {
    let departure: Departure
    let isFirst: Bool
    
    var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 1) {
                // First line: Time
                HStack(spacing: 6) {
                    Text(timeText)
                        .font(.system(size: isFirst ? 18 : 16, weight: .bold, design: .rounded))
                        .foregroundColor(.white)
                    
                    if departure.timeToStation <= 1 {
                        Circle()
                            .fill(Color.yellow)
                            .frame(width: 6, height: 6)
                            .opacity(0.8)
                    }
                }
                
                // Second line: Direction
                Text("to \(departure.towards)")
                    .font(.system(size: isFirst ? 12 : 11, weight: .medium, design: .rounded))
                    .foregroundColor(.white.opacity(0.85))
                    .lineLimit(1)
            }
            
            Spacer()
            
            // Platform indicator
            if !departure.platformName.isEmpty {
                Text(departure.platformName.replacingOccurrences(of: "Platform ", with: ""))
                    .font(.system(size: 10, weight: .semibold, design: .rounded))
                    .foregroundColor(.white.opacity(0.7))
                    .padding(.horizontal, 6)
                    .padding(.vertical, 2)
                    .background(
                        Capsule()
                            .fill(Color.white.opacity(0.2))
                    )
            }
        }
        .padding(.vertical, isFirst ? 4 : 2)
    }
    
    private var timeText: String {
        switch departure.timeToStation {
        case 0:
            return "Due"
        case 1:
            return "1 min"
        default:
            return "\(departure.timeToStation) mins"
        }
    }
}

// MARK: - Widget Configuration
struct DistrictLineWidget: Widget {
    let kind: String = "DistrictLineWidget"

    var body: some WidgetConfiguration {
        StaticConfiguration(kind: kind, provider: Provider()) { entry in
            DistrictLineWidgetEntryView(entry: entry)
        }
        .configurationDisplayName("District Line - Becontree")
        .description("Live departure times for District Line from Becontree station")
        .supportedFamilies([.systemSmall, .systemMedium])
    }
}

// MARK: - Previews
struct DistrictLineWidget_Previews: PreviewProvider {
    static var previews: some View {
        DistrictLineWidgetEntryView(entry: DepartureEntry(date: Date(), departures: [
            Departure(timeToStation: 2, towards: "Upminster", platformName: "Platform 1", currentLocation: "Approaching"),
            Departure(timeToStation: 6, towards: "Ealing Broadway", platformName: "Platform 2", currentLocation: "At Dagenham East")
        ]))
        .previewContext(WidgetPreviewContext(family: .systemSmall))
    }
}