#!/usr/bin/env python3
"""
Usage Examples for Image URL Generator
Demonstrates various ways to generate and use image URLs
"""

from image_url_generator import ImageURLGenerator
import os

def example_basic_usage():
    """Basic usage example"""
    print("=== Basic Usage Example ===")
    
    generator = ImageURLGenerator()
    
    # Assume we have an image file
    image_path = "sample_image.jpg"
    
    if os.path.exists(image_path):
        # Generate different URL types
        file_url = generator.file_url(image_path)
        data_url = generator.data_url(image_path)
        web_url = generator.web_url(image_path)
        
        print(f"File URL: {file_url}")
        print(f"Data URL: {data_url[:50]}...")
        print(f"Web URL: {web_url}")
    else:
        print(f"Image not found: {image_path}")

def example_web_server():
    """Example of using with a web server"""
    print("\n=== Web Server Example ===")
    
    generator = ImageURLGenerator()
    
    try:
        # Start a local server
        server_url = generator.start_local_server(port=8080)
        print(f"Server running at: {server_url}")
        
        # You can now access images via the web server
        # Example: http://localhost:8080/your_image.jpg
        
        input("Press Enter to stop the server...")
        generator.stop_local_server()
        
    except Exception as e:
        print(f"Server error: {e}")

def example_html_generation():
    """Example of generating HTML viewer"""
    print("\n=== HTML Generation Example ===")
    
    generator = ImageURLGenerator()
    image_path = "sample_image.jpg"
    
    if os.path.exists(image_path):
        html_file = generator.create_html_viewer(image_path, "my_image_viewer.html")
        print(f"HTML viewer created: {html_file}")
        print("Open this file in your browser to view the image and URLs")
    else:
        print(f"Image not found: {image_path}")

def example_batch_processing():
    """Example of processing multiple images"""
    print("\n=== Batch Processing Example ===")
    
    generator = ImageURLGenerator()
    
    # List of image files (you can modify this to your actual images)
    image_files = ["image1.jpg", "image2.png", "image3.gif"]
    
    for image_file in image_files:
        if os.path.exists(image_file):
            print(f"\nProcessing: {image_file}")
            urls = generator.generate_all_urls(image_file)
            
            for url_type, url in urls.items():
                if url_type == 'data_url':
                    print(f"  {url_type}: {url[:50]}...")
                else:
                    print(f"  {url_type}: {url}")
        else:
            print(f"Skipping missing file: {image_file}")

def example_custom_web_root():
    """Example using custom web root directory"""
    print("\n=== Custom Web Root Example ===")
    
    generator = ImageURLGenerator("https://mywebsite.com")
    
    # If you have images in subdirectories
    web_root = "/var/www/html"
    image_path = "/var/www/html/images/photo.jpg"
    
    if os.path.exists(image_path):
        web_url = generator.web_url(image_path, relative_to_webroot=web_root)
        print(f"Web URL: {web_url}")
    else:
        print("Using hypothetical paths for demonstration")
        print("Web URL would be: https://mywebsite.com/images/photo.jpg")

def example_inline_image():
    """Example of creating inline image for email or documents"""
    print("\n=== Inline Image Example ===")
    
    generator = ImageURLGenerator()
    image_path = "sample_image.jpg"
    
    if os.path.exists(image_path):
        data_url = generator.data_url(image_path)
        
        # Create HTML with inline image
        html_content = f'''
        <html>
        <body>
            <h1>Email with Inline Image</h1>
            <p>This image is embedded directly in the HTML:</p>
            <img src="{data_url}" alt="Inline Image" style="max-width: 300px;">
        </body>
        </html>
        '''
        
        with open("email_with_image.html", "w") as f:
            f.write(html_content)
        
        print("Created email_with_image.html with inline image")
        print(f"Data URL length: {len(data_url)} characters")
    else:
        print(f"Image not found: {image_path}")

if __name__ == "__main__":
    print("Image URL Generator - Usage Examples")
    print("=" * 50)
    
    # Run all examples
    example_basic_usage()
    example_html_generation()
    example_batch_processing()
    example_custom_web_root()
    example_inline_image()
    
    # Uncomment the line below to run the web server example
    # example_web_server()
    
    print("\n" + "=" * 50)
    print("Examples completed!")
    print("Check the generated HTML files to see the results.")