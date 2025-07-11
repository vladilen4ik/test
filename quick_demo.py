#!/usr/bin/env python3
"""
Quick Demo - Image URL Generator
Creates a sample image and demonstrates all URL generation features
"""

import os
import sys
from PIL import Image, ImageDraw, ImageFont

def create_demo_image():
    """Create a simple demo image for testing"""
    # Create a colorful demo image
    width, height = 400, 300
    img = Image.new('RGB', (width, height), color='lightblue')
    draw = ImageDraw.Draw(img)
    
    # Draw some shapes and text
    draw.rectangle([50, 50, 350, 250], fill='white', outline='navy', width=3)
    draw.ellipse([100, 100, 200, 200], fill='lightgreen', outline='darkgreen', width=2)
    draw.polygon([(250, 120), (320, 120), (285, 180)], fill='orange', outline='red', width=2)
    
    # Add text
    try:
        # Try to use a better font, fall back to default if not available
        font = ImageFont.truetype("arial.ttf", 20)
    except:
        font = ImageFont.load_default()
    
    draw.text((150, 220), "Demo Image", fill='black', font=font)
    draw.text((120, 240), "URL Generator Test", fill='navy', font=font)
    
    # Save the image
    filename = "demo_image.jpg"
    img.save(filename, "JPEG", quality=85)
    print(f"✅ Created demo image: {filename}")
    return filename

def main():
    """Run the quick demo"""
    print("🖼️  Image URL Generator - Quick Demo")
    print("=" * 50)
    
    # Create demo image
    image_path = create_demo_image()
    
    # Import our generator (handling potential import issues)
    try:
        from image_url_generator import ImageURLGenerator
    except ImportError:
        print("❌ Error: Could not import ImageURLGenerator")
        print("Make sure image_url_generator.py is in the same directory")
        return
    
    # Initialize generator
    generator = ImageURLGenerator()
    
    print(f"\n📁 Generating URLs for: {image_path}")
    print("-" * 30)
    
    try:
        # Generate all URL types
        urls = generator.generate_all_urls(image_path)
        
        print(f"🔗 File URL:")
        print(f"   {urls['file_url']}")
        
        print(f"\n🌐 Web URL:")
        print(f"   {urls['web_url']}")
        
        print(f"\n📊 Data URL:")
        print(f"   {urls['data_url'][:60]}... [{len(urls['data_url'])} chars total]")
        
        # Create HTML viewer
        print(f"\n🎨 Creating HTML viewer...")
        html_file = generator.create_html_viewer(image_path, "demo_viewer.html")
        print(f"   Created: {os.path.basename(html_file)}")
        
        # Start local server
        print(f"\n🚀 Starting local web server...")
        try:
            server_url = generator.start_local_server(port=8000)
            print(f"   Server running at: {server_url}")
            print(f"   View demo at: {server_url}/demo_viewer.html")
            print(f"   Direct image at: {server_url}/{image_path}")
            
            print(f"\n⏸️  Press Enter to stop the server and exit...")
            input()
            
            generator.stop_local_server()
            print("✅ Server stopped")
            
        except Exception as e:
            print(f"⚠️  Server error: {e}")
            print("HTML viewer still created - you can open it directly in your browser")
    
    except Exception as e:
        print(f"❌ Error: {e}")
        return
    
    print(f"\n📋 Summary:")
    print(f"   • Demo image: {image_path}")
    print(f"   • HTML viewer: demo_viewer.html")
    print(f"   • All URL types generated successfully!")
    
    print(f"\n🎯 Try these next:")
    print(f"   • python usage_examples.py")
    print(f"   • python image_url_generator.py")
    print(f"   • Open demo_viewer.html in your browser")

if __name__ == "__main__":
    main()