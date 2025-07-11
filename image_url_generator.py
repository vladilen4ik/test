#!/usr/bin/env python3
"""
Image URL Generator
Provides multiple methods to generate URLs for viewing images
"""

import os
import base64
import mimetypes
from pathlib import Path
from urllib.parse import urljoin
from http.server import HTTPServer, SimpleHTTPRequestHandler
import threading
import webbrowser

class ImageURLGenerator:
    def __init__(self, base_url="http://localhost:8000"):
        self.base_url = base_url
        self.server = None
        self.server_thread = None
    
    def file_url(self, image_path):
        """Generate a file:// URL for local images"""
        abs_path = os.path.abspath(image_path)
        return f"file://{abs_path}"
    
    def data_url(self, image_path):
        """Generate a data URL (base64 encoded) for images"""
        if not os.path.exists(image_path):
            raise FileNotFoundError(f"Image not found: {image_path}")
        
        # Get MIME type
        mime_type, _ = mimetypes.guess_type(image_path)
        if not mime_type or not mime_type.startswith('image/'):
            mime_type = 'image/jpeg'  # Default fallback
        
        # Read and encode image
        with open(image_path, 'rb') as image_file:
            encoded_string = base64.b64encode(image_file.read()).decode('utf-8')
        
        return f"data:{mime_type};base64,{encoded_string}"
    
    def web_url(self, image_path, relative_to_webroot=None):
        """Generate a web URL assuming the image is served by a web server"""
        if relative_to_webroot:
            rel_path = os.path.relpath(image_path, relative_to_webroot)
        else:
            rel_path = os.path.basename(image_path)
        
        return urljoin(self.base_url + "/", rel_path)
    
    def start_local_server(self, directory=".", port=8000):
        """Start a local HTTP server to serve images"""
        os.chdir(directory)
        
        class QuietHTTPRequestHandler(SimpleHTTPRequestHandler):
            def log_message(self, format, *args):
                pass  # Suppress log messages
        
        self.server = HTTPServer(('localhost', port), QuietHTTPRequestHandler)
        self.server_thread = threading.Thread(target=self.server.serve_forever)
        self.server_thread.daemon = True
        self.server_thread.start()
        
        self.base_url = f"http://localhost:{port}"
        print(f"Local server started at {self.base_url}")
        return self.base_url
    
    def stop_local_server(self):
        """Stop the local HTTP server"""
        if self.server:
            self.server.shutdown()
            self.server = None
            print("Local server stopped")
    
    def generate_all_urls(self, image_path):
        """Generate all possible URL formats for an image"""
        urls = {
            'file_url': self.file_url(image_path),
            'data_url': self.data_url(image_path),
            'web_url': self.web_url(image_path)
        }
        return urls
    
    def create_html_viewer(self, image_path, output_file="image_viewer.html"):
        """Create an HTML file to view the image with all URL types"""
        urls = self.generate_all_urls(image_path)
        
        html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Image Viewer - {os.path.basename(image_path)}</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f5f5f5;
        }}
        .container {{
            background: white;
            border-radius: 8px;
            padding: 30px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        .image-display {{
            text-align: center;
            margin: 30px 0;
        }}
        .image-display img {{
            max-width: 100%;
            max-height: 500px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
        }}
        .url-section {{
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 5px;
            border-left: 4px solid #007bff;
        }}
        .url-type {{
            font-weight: bold;
            color: #007bff;
            margin-bottom: 8px;
        }}
        .url-text {{
            font-family: monospace;
            background: #e9ecef;
            padding: 8px;
            border-radius: 3px;
            word-break: break-all;
            font-size: 12px;
        }}
        .copy-btn {{
            background: #007bff;
            color: white;
            border: none;
            padding: 5px 10px;
            border-radius: 3px;
            cursor: pointer;
            font-size: 12px;
            margin-top: 5px;
        }}
        .copy-btn:hover {{
            background: #0056b3;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Image Viewer: {os.path.basename(image_path)}</h1>
        
        <div class="image-display">
            <img src="{urls['data_url']}" alt="{os.path.basename(image_path)}">
        </div>
        
        <div class="url-section">
            <div class="url-type">File URL (Local filesystem access)</div>
            <div class="url-text">{urls['file_url']}</div>
            <button class="copy-btn" onclick="copyToClipboard('{urls['file_url']}')">Copy URL</button>
        </div>
        
        <div class="url-section">
            <div class="url-type">Web URL (HTTP server access)</div>
            <div class="url-text">{urls['web_url']}</div>
            <button class="copy-btn" onclick="copyToClipboard('{urls['web_url']}')">Copy URL</button>
        </div>
        
        <div class="url-section">
            <div class="url-type">Data URL (Base64 encoded - works anywhere)</div>
            <div class="url-text">{urls['data_url'][:100]}...</div>
            <button class="copy-btn" onclick="copyToClipboard('{urls['data_url']}')">Copy Full Data URL</button>
        </div>
    </div>
    
    <script>
        function copyToClipboard(text) {{
            navigator.clipboard.writeText(text).then(function() {{
                alert('URL copied to clipboard!');
            }}, function(err) {{
                console.error('Could not copy text: ', err);
            }});
        }}
    </script>
</body>
</html>"""
        
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        return os.path.abspath(output_file)

def main():
    """Demo usage of the ImageURLGenerator"""
    generator = ImageURLGenerator()
    
    print("Image URL Generator Demo")
    print("=" * 40)
    
    # Example usage
    image_path = input("Enter image path (or press Enter for demo): ").strip()
    
    if not image_path:
        # Create a demo image for testing
        print("Creating a demo image...")
        import io
        from PIL import Image, ImageDraw
        
        # Create a simple demo image
        img = Image.new('RGB', (400, 300), color='lightblue')
        draw = ImageDraw.Draw(img)
        draw.text((150, 140), "Demo Image", fill='black')
        img.save('demo_image.jpg')
        image_path = 'demo_image.jpg'
        print(f"Created demo image: {image_path}")
    
    if not os.path.exists(image_path):
        print(f"Error: Image not found at {image_path}")
        return
    
    print(f"\nGenerating URLs for: {image_path}")
    print("-" * 40)
    
    try:
        # Generate all URL types
        urls = generator.generate_all_urls(image_path)
        
        print(f"File URL: {urls['file_url']}")
        print(f"Web URL: {urls['web_url']}")
        print(f"Data URL: {urls['data_url'][:80]}...")
        
        # Start local server
        print("\nStarting local web server...")
        server_url = generator.start_local_server()
        
        # Create HTML viewer
        html_file = generator.create_html_viewer(image_path)
        print(f"Created HTML viewer: {html_file}")
        
        # Open in browser
        viewer_url = f"{server_url}/image_viewer.html"
        print(f"Opening viewer at: {viewer_url}")
        webbrowser.open(viewer_url)
        
        input("Press Enter to stop the server...")
        generator.stop_local_server()
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()