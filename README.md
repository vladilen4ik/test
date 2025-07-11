# Image URL Generator

A comprehensive toolkit for generating URLs to view images in various formats. This project provides both Python and JavaScript implementations for creating file URLs, data URLs, web URLs, and setting up local servers to serve images.

## Features

- **Multiple URL Types**: Generate file://, data:, and web URLs for images
- **Local Web Server**: Built-in HTTP server for serving images locally
- **HTML Viewer**: Auto-generate beautiful HTML pages to display images with all URL types
- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Multiple Languages**: Python and JavaScript/Node.js implementations
- **Browser Support**: Client-side JavaScript version for web applications

## Quick Start

### Python Version

1. **Install dependencies:**
   ```bash
   pip install -r requirements.txt
   ```

2. **Basic usage:**
   ```python
   from image_url_generator import ImageURLGenerator
   
   generator = ImageURLGenerator()
   
   # Generate all URL types for an image
   urls = generator.generate_all_urls("my_image.jpg")
   print(urls['file_url'])    # file:///path/to/my_image.jpg
   print(urls['data_url'])    # data:image/jpeg;base64,/9j/4AAQ...
   print(urls['web_url'])     # http://localhost:8000/my_image.jpg
   ```

3. **Run the demo:**
   ```bash
   python image_url_generator.py
   ```

### JavaScript/Node.js Version

1. **Install dependencies:**
   ```bash
   npm install
   ```

2. **Basic usage:**
   ```javascript
   const { ImageURLGenerator } = require('./image_url_generator');
   
   const generator = new ImageURLGenerator();
   const urls = generator.generateAllUrls('my_image.jpg');
   console.log(urls);
   ```

3. **Run the demo:**
   ```bash
   npm start my_image.jpg
   ```

## URL Types Explained

### 1. File URLs (`file://`)
- **Use case**: Local development, desktop applications
- **Format**: `file:///absolute/path/to/image.jpg`
- **Pros**: Direct file system access, no server needed
- **Cons**: Only works locally, security restrictions in browsers

### 2. Data URLs (Base64)
- **Use case**: Emails, embedded images, offline applications
- **Format**: `data:image/jpeg;base64,/9j/4AAQSkZJRgABA...`
- **Pros**: Works anywhere, no external dependencies
- **Cons**: Large size (33% larger than original), not cacheable

### 3. Web URLs (`http://` or `https://`)
- **Use case**: Web applications, sharing, production environments
- **Format**: `http://localhost:8000/image.jpg`
- **Pros**: Cacheable, shareable, standard web format
- **Cons**: Requires web server, network dependency

## Advanced Usage

### Starting a Local Server

**Python:**
```python
generator = ImageURLGenerator()
server_url = generator.start_local_server(directory="./images", port=8080)
print(f"Server running at: {server_url}")

# Access images at: http://localhost:8080/filename.jpg
```

**JavaScript:**
```javascript
const generator = new ImageURLGenerator();
await generator.startLocalServer('./images', 3000);
console.log('Server running at: http://localhost:3000');
```

### Creating HTML Viewers

Both versions can generate beautiful HTML pages that display your image with all URL types:

```python
# Python
html_file = generator.create_html_viewer("image.jpg", "viewer.html")
```

```javascript
// JavaScript
const htmlFile = generator.createHtmlViewer('image.jpg', 'viewer.html');
```

### Browser Usage (JavaScript)

For client-side web applications:

```html
<script src="image_url_generator.js"></script>
<script>
const generator = new ImageURLGenerator();

// Handle file input
document.getElementById('fileInput').addEventListener('change', async (e) => {
    const file = e.target.files[0];
    if (file) {
        const urls = await generator.createImagePreview(file, 'preview-container');
        console.log('Blob URL:', urls.blobUrl);
        console.log('Data URL:', urls.dataUrl);
    }
});
</script>
```

## Examples

Check out `usage_examples.py` for comprehensive examples including:

- Basic URL generation
- Web server setup
- Batch processing multiple images
- Custom web root configurations
- Inline images for emails
- HTML generation

## API Reference

### Python - ImageURLGenerator

#### Methods

- `file_url(image_path)` - Generate file:// URL
- `data_url(image_path)` - Generate data: URL (base64)
- `web_url(image_path, relative_to_webroot=None)` - Generate web URL
- `start_local_server(directory=".", port=8000)` - Start HTTP server
- `stop_local_server()` - Stop HTTP server
- `generate_all_urls(image_path)` - Generate all URL types
- `create_html_viewer(image_path, output_file)` - Create HTML viewer

### JavaScript - ImageURLGenerator

#### Methods

- `fileUrl(imagePath)` - Generate file:// URL
- `dataUrl(imagePath)` - Generate data: URL (base64)
- `webUrl(imagePath, relativeToWebroot)` - Generate web URL
- `startLocalServer(directory, port)` - Start Express server
- `stopLocalServer()` - Stop Express server
- `generateAllUrls(imagePath)` - Generate all URL types
- `createHtmlViewer(imagePath, outputFile)` - Create HTML viewer

### Browser - BrowserImageURLGenerator

#### Methods

- `blobUrl(file)` - Generate blob: URL from File object
- `dataUrlFromFile(file)` - Generate data: URL from File object
- `webUrl(filename)` - Generate web URL for uploads
- `createImagePreview(file, container)` - Create image preview in DOM

## Dependencies

### Python
- `Pillow` - Image processing (for demo image generation)

### Node.js
- `express` - Web server framework

### Browser
- No external dependencies (vanilla JavaScript)

## File Structure

```
├── image_url_generator.py      # Python implementation
├── image_url_generator.js      # JavaScript/Node.js implementation  
├── usage_examples.py           # Python usage examples
├── requirements.txt            # Python dependencies
├── package.json               # Node.js dependencies
├── README.md                  # This file
└── index.html                 # Basic HTML template
```

## Use Cases

1. **Web Development**: Generate URLs for user-uploaded images
2. **Desktop Applications**: Display local images with various URL formats
3. **Email Templates**: Embed images using data URLs
4. **Documentation**: Create image galleries with multiple viewing options
5. **Development Tools**: Quick image preview and URL generation
6. **Testing**: Local server for testing image loading scenarios

## Browser Compatibility

- **File URLs**: Modern browsers with security restrictions
- **Data URLs**: All modern browsers
- **Web URLs**: All browsers
- **Blob URLs**: All modern browsers (for client-side usage)

## Security Considerations

- File URLs have security restrictions in modern browsers
- Data URLs can be very large and may hit browser limits
- Local servers should only be used for development
- Validate image file types and sizes in production

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this toolkit.

## License

MIT License - feel free to use this in your projects!