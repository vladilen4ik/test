#!/usr/bin/env python3
"""
Simple Demo - Image URL Generator (No Dependencies)
Demonstrates URL generation features without requiring external libraries
"""

import os
import base64
import mimetypes
from urllib.parse import urljoin

def create_demo_data():
    """Create a simple demo file for testing"""
    # Create a simple SVG image (text-based, no external dependencies)
    svg_content = '''<?xml version="1.0" encoding="UTF-8"?>
<svg width="400" height="300" xmlns="http://www.w3.org/2000/svg">
  <rect width="400" height="300" fill="lightblue"/>
  <rect x="50" y="50" width="300" height="200" fill="white" stroke="navy" stroke-width="3"/>
  <circle cx="150" cy="150" r="50" fill="lightgreen" stroke="darkgreen" stroke-width="2"/>
  <polygon points="250,120 320,120 285,180" fill="orange" stroke="red" stroke-width="2"/>
  <text x="150" y="220" font-family="Arial, sans-serif" font-size="20" fill="black">Demo Image</text>
  <text x="120" y="240" font-family="Arial, sans-serif" font-size="16" fill="navy">URL Generator Test</text>
</svg>'''
    
    filename = "demo_image.svg"
    with open(filename, 'w') as f:
        f.write(svg_content)
    print(f"✅ Created demo SVG: {filename}")
    return filename

def file_url(image_path):
    """Generate a file:// URL for local images"""
    abs_path = os.path.abspath(image_path)
    return f"file://{abs_path}"

def data_url(image_path):
    """Generate a data URL (base64 encoded) for images"""
    if not os.path.exists(image_path):
        raise FileNotFoundError(f"File not found: {image_path}")
    
    # Get MIME type
    mime_type, _ = mimetypes.guess_type(image_path)
    if not mime_type:
        mime_type = 'image/svg+xml' if image_path.endswith('.svg') else 'application/octet-stream'
    
    # Read and encode image
    with open(image_path, 'rb') as image_file:
        encoded_string = base64.b64encode(image_file.read()).decode('utf-8')
    
    return f"data:{mime_type};base64,{encoded_string}"

def web_url(image_path, base_url="http://localhost:8000"):
    """Generate a web URL for images served by a web server"""
    rel_path = os.path.basename(image_path)
    return urljoin(base_url + "/", rel_path)

def create_html_viewer(image_path, output_file="simple_viewer.html"):
    """Create an HTML file to view the image with all URL types"""
    file_url_val = file_url(image_path)
    data_url_val = data_url(image_path)
    web_url_val = web_url(image_path)
    
    html_content = f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple Image Viewer - {os.path.basename(image_path)}</title>
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }}
        .container {{
            background: white;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }}
        .header {{
            text-align: center;
            margin-bottom: 30px;
        }}
        .image-display {{
            text-align: center;
            margin: 30px 0;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 10px;
        }}
        .image-display img {{
            max-width: 100%;
            max-height: 400px;
            border-radius: 10px;
            box-shadow: 0 8px 16px rgba(0,0,0,0.2);
        }}
        .url-section {{
            margin: 20px 0;
            padding: 20px;
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            border-radius: 10px;
            border-left: 5px solid #007bff;
        }}
        .url-type {{
            font-weight: bold;
            color: #007bff;
            margin-bottom: 10px;
            font-size: 16px;
        }}
        .url-description {{
            color: #666;
            font-size: 14px;
            margin-bottom: 10px;
        }}
        .url-text {{
            font-family: 'Courier New', monospace;
            background: #fff;
            padding: 12px;
            border-radius: 5px;
            word-break: break-all;
            font-size: 12px;
            border: 1px solid #e9ecef;
            max-height: 100px;
            overflow-y: auto;
        }}
        .copy-btn {{
            background: linear-gradient(135deg, #007bff 0%, #0056b3 100%);
            color: white;
            border: none;
            padding: 8px 15px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 12px;
            margin-top: 10px;
        }}
        .copy-btn:hover {{
            transform: translateY(-1px);
            box-shadow: 0 3px 10px rgba(0,123,255,0.3);
        }}
        .success {{
            color: #28a745;
            font-size: 12px;
            margin-top: 5px;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🖼️ Simple Image Viewer</h1>
            <p><strong>{os.path.basename(image_path)}</strong></p>
        </div>
        
        <div class="image-display">
            <img src="{data_url_val}" alt="{os.path.basename(image_path)}">
        </div>
        
        <div class="url-section">
            <div class="url-type">📁 File URL</div>
            <div class="url-description">Direct filesystem access (works locally)</div>
            <div class="url-text">{file_url_val}</div>
            <button class="copy-btn" onclick="copyToClipboard('{file_url_val.replace("'", "\\'")}')">Copy URL</button>
        </div>
        
        <div class="url-section">
            <div class="url-type">🌐 Web URL</div>
            <div class="url-description">HTTP server access (shareable link)</div>
            <div class="url-text">{web_url_val}</div>
            <button class="copy-btn" onclick="copyToClipboard('{web_url_val}')">Copy URL</button>
        </div>
        
        <div class="url-section">
            <div class="url-type">📊 Data URL</div>
            <div class="url-description">Base64 encoded (embedded, works anywhere)</div>
            <div class="url-text">{data_url_val[:100]}...</div>
            <button class="copy-btn" onclick="copyToClipboard('{data_url_val.replace("'", "\\'")}')">Copy Full Data URL</button>
        </div>
    </div>
    
    <script>
        async function copyToClipboard(text) {{
            try {{
                await navigator.clipboard.writeText(text);
                alert('URL copied to clipboard!');
            }} catch (err) {{
                console.error('Could not copy text: ', err);
                // Fallback for older browsers
                const textArea = document.createElement('textarea');
                textArea.value = text;
                document.body.appendChild(textArea);
                textArea.select();
                document.execCommand('copy');
                document.body.removeChild(textArea);
                alert('URL copied to clipboard!');
            }}
        }}
    </script>
</body>
</html>'''
    
    with open(output_file, 'w') as f:
        f.write(html_content)
    
    return os.path.abspath(output_file)

def main():
    """Run the simple demo"""
    print("🖼️  Image URL Generator - Simple Demo")
    print("=" * 50)
    
    # Create demo file
    image_path = create_demo_data()
    
    print(f"\n📁 Generating URLs for: {image_path}")
    print("-" * 30)
    
    try:
        # Generate all URL types
        file_url_val = file_url(image_path)
        data_url_val = data_url(image_path)
        web_url_val = web_url(image_path)
        
        print(f"🔗 File URL:")
        print(f"   {file_url_val}")
        
        print(f"\n🌐 Web URL:")
        print(f"   {web_url_val}")
        
        print(f"\n📊 Data URL:")
        print(f"   {data_url_val[:60]}... [{len(data_url_val)} chars total]")
        
        # Create HTML viewer
        print(f"\n🎨 Creating HTML viewer...")
        html_file = create_html_viewer(image_path, "simple_viewer.html")
        print(f"   Created: {os.path.basename(html_file)}")
        
        print(f"\n📋 Summary:")
        print(f"   • Demo SVG: {image_path}")
        print(f"   • HTML viewer: simple_viewer.html")
        print(f"   • All URL types generated successfully!")
        
        print(f"\n🎯 To test:")
        print(f"   • Open simple_viewer.html in your browser")
        print(f"   • Check that the SVG image displays correctly")
        print(f"   • Try copying the different URL types")
        
        print(f"\n💡 Note: This demo uses SVG format which works without external dependencies!")
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return

if __name__ == "__main__":
    main()