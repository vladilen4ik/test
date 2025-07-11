# Getting Started with Image URL Generator

## 🚀 Quick Start

This toolkit provides multiple ways to generate URLs for viewing images. Here's how to get started immediately:

### ⚡ Instant Demo

Run this command to see the toolkit in action:

```bash
python3 simple_demo.py
```

This will:
- ✅ Create a demo SVG image
- ✅ Generate file://, web, and data URLs
- ✅ Create a beautiful HTML viewer
- ✅ Show you all URL types with copy functionality

### 📁 What Was Created

| File | Purpose |
|------|---------|
| `image_url_generator.py` | Main Python implementation with full features |
| `image_url_generator.js` | JavaScript/Node.js implementation |
| `simple_demo.py` | Quick demo (no external dependencies) |
| `quick_demo.py` | Full-featured demo (requires Pillow) |
| `usage_examples.py` | Comprehensive usage examples |
| `simple_viewer.html` | Generated HTML viewer |
| `demo_image.svg` | Demo SVG image |

## 🎯 Three Ways to Use This

### 1. **Simple Python Usage (Recommended Start)**

```python
# Copy this into your Python script
import os
import base64
import mimetypes

def generate_image_urls(image_path):
    """Generate all URL types for an image"""
    
    # File URL
    file_url = f"file://{os.path.abspath(image_path)}"
    
    # Data URL
    with open(image_path, 'rb') as f:
        data = base64.b64encode(f.read()).decode()
    mime_type = mimetypes.guess_type(image_path)[0] or 'application/octet-stream'
    data_url = f"data:{mime_type};base64,{data}"
    
    # Web URL
    web_url = f"http://localhost:8000/{os.path.basename(image_path)}"
    
    return {
        'file_url': file_url,
        'data_url': data_url,
        'web_url': web_url
    }

# Usage
urls = generate_image_urls("your_image.jpg")
print(urls['file_url'])    # file:///path/to/your_image.jpg
print(urls['data_url'])    # data:image/jpeg;base64,/9j/4AAQ...
print(urls['web_url'])     # http://localhost:8000/your_image.jpg
```

### 2. **Full Python Implementation**

```python
from image_url_generator import ImageURLGenerator

generator = ImageURLGenerator()

# Generate all URL types
urls = generator.generate_all_urls("my_image.jpg")

# Start a local server
server_url = generator.start_local_server(port=8080)

# Create HTML viewer
html_file = generator.create_html_viewer("my_image.jpg", "viewer.html")
```

### 3. **JavaScript/Web Usage**

```javascript
// For Node.js
const { ImageURLGenerator } = require('./image_url_generator');
const generator = new ImageURLGenerator();
const urls = generator.generateAllUrls('image.jpg');

// For browsers
const generator = new ImageURLGenerator();
const urls = await generator.createImagePreview(fileInput.files[0], 'container');
```

## 🌟 URL Types Explained

### 📁 File URLs (`file://`)
- **Best for**: Local development, desktop apps
- **Example**: `file:///Users/you/image.jpg`
- **Pros**: Direct access, no server needed
- **Cons**: Browser security restrictions

### 📊 Data URLs (Base64)
- **Best for**: Emails, embedding, offline use
- **Example**: `data:image/jpeg;base64,/9j/4AAQ...`
- **Pros**: Works everywhere, self-contained
- **Cons**: Large size (33% bigger)

### 🌐 Web URLs (`http://`)
- **Best for**: Web apps, sharing, production
- **Example**: `http://localhost:8000/image.jpg`
- **Pros**: Standard, cacheable, shareable
- **Cons**: Requires web server

## 🎨 View Your Results

After running any demo, open the generated HTML file in your browser:

```bash
# Opens the beautiful HTML viewer
open simple_viewer.html    # macOS
xdg-open simple_viewer.html    # Linux
start simple_viewer.html    # Windows
```

## 🔧 Common Use Cases

### Generate URL for Email
```python
from image_url_generator import ImageURLGenerator
generator = ImageURLGenerator()
data_url = generator.data_url("logo.png")
# Use data_url in your email HTML
```

### Share Image Online
```python
generator = ImageURLGenerator()
generator.start_local_server(port=8080)
# Share: http://localhost:8080/your_image.jpg
```

### Embed in Documentation
```python
generator = ImageURLGenerator()
html_viewer = generator.create_html_viewer("screenshot.png", "docs.html")
# Include docs.html in your documentation
```

## 🚀 Next Steps

1. **Try the demo**: `python3 simple_demo.py`
2. **Read examples**: Open `usage_examples.py`
3. **Use in your project**: Copy functions you need
4. **For web apps**: Use the JavaScript version
5. **For production**: Set up proper web server

## 💡 Tips

- **SVG images** work great (small, scalable)
- **Data URLs** are perfect for small images (<100KB)
- **File URLs** have browser security restrictions
- **Web URLs** need a running server

## 🆘 Troubleshooting

**"Module not found" error?**
- Use `simple_demo.py` (no dependencies)
- Or install: `pip install Pillow`

**Browser won't load file:// URLs?**
- Use the data URL instead
- Or start the local server

**Large data URLs?**
- Use web URLs for big images
- Consider image compression

## ✨ Features You Get

- ✅ Multiple URL formats
- ✅ Beautiful HTML viewers  
- ✅ Copy-to-clipboard functionality
- ✅ Local web server
- ✅ Cross-platform support
- ✅ No external dependencies (simple version)
- ✅ Both Python and JavaScript
- ✅ Browser compatibility

---

**Ready to generate some image URLs?** Start with `python3 simple_demo.py` and see the magic! 🎉