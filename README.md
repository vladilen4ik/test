# District Line Widget for iOS

An iOS widget that displays live departure times for the District Line from Becontree station.

## Features

- **Live Departure Times**: Fetches real-time data from Transport for London (TfL) API
- **Two-Line Display**: 
  - First line shows departure time (e.g., "3 mins", "Due")
  - Second line shows direction (e.g., "to Upminster")
- **District Line Branding**: 
  - Authentic green color scheme matching TfL District Line
  - TfL roundel-inspired design elements
- **Smart Updates**: Automatically refreshes every 30 seconds
- **Fallback Data**: Shows sample departure times when API is unavailable

## Design

The widget follows Transport for London's design language:
- **Colors**: Official District Line green (#007241)
- **Typography**: Rounded system font for modern readability
- **Layout**: Clean two-line format as requested
- **Icons**: TfL roundel-inspired branding element
- **Platform Indicators**: Shows platform numbers when available

## Widget Configuration

- **Station**: Becontree (District Line)
- **Supported Sizes**: Small and Medium iOS widgets
- **Update Frequency**: Every 30 seconds
- **Data Source**: TfL Unified API

## Technical Details

- **Platform**: iOS 17.0+
- **Framework**: WidgetKit with SwiftUI
- **API**: Transport for London Unified API
- **Station ID**: 940GZZLUBCT (Becontree)
- **Line**: District Line

## Project Structure

```
DistrictLineWidget.xcodeproj/
├── DistrictLineWidget/              # Main iOS app
│   ├── AppDelegate.swift
│   ├── SceneDelegate.swift
│   ├── ViewController.swift
│   ├── Assets.xcassets/
│   └── Base.lproj/
└── DistrictLineWidgetExtension/     # Widget extension
    ├── DistrictLineWidget.swift     # Main widget implementation
    ├── DistrictLineWidgetExtension.swift
    ├── Assets.xcassets/
    └── Info.plist
```

## How to Use

1. Open the project in Xcode
2. Build and run on an iOS device or simulator
3. Add the widget to your home screen:
   - Long press on the home screen
   - Tap the "+" button
   - Search for "District Line"
   - Select your preferred widget size
   - Add to home screen

## Data Format

The widget displays up to 2 upcoming departures with:
- **Time**: Minutes until departure ("Due", "1 min", "5 mins")
- **Direction**: Destination ("to Upminster", "to Ealing Broadway")
- **Platform**: When available ("Platform 1", "Platform 2")
- **Visual Indicators**: Yellow dot for trains arriving in ≤1 minute

## Notes

- Requires internet connection for live data
- Falls back to sample data when TfL API is unavailable
- Updates automatically every 30 seconds
- Designed specifically for Becontree station on the District Line
- Background uses authentic TfL District Line green color scheme