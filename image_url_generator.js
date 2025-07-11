/**
 * Image URL Generator - JavaScript/Node.js Version
 * Provides multiple methods to generate URLs for viewing images in web applications
 */

const fs = require('fs');
const path = require('path');
const http = require('http');
const express = require('express');

class ImageURLGenerator {
    constructor(baseUrl = 'http://localhost:3000') {
        this.baseUrl = baseUrl;
        this.server = null;
    }

    /**
     * Generate a file URL for local images (browser file:// protocol)
     */
    fileUrl(imagePath) {
        const absolutePath = path.resolve(imagePath);
        return `file://${absolutePath}`;
    }

    /**
     * Generate a data URL (base64 encoded) for images
     */
    dataUrl(imagePath) {
        if (!fs.existsSync(imagePath)) {
            throw new Error(`Image not found: ${imagePath}`);
        }

        const imageBuffer = fs.readFileSync(imagePath);
        const mimeType = this.getMimeType(imagePath);
        const base64String = imageBuffer.toString('base64');
        
        return `data:${mimeType};base64,${base64String}`;
    }

    /**
     * Generate a web URL for images served by a web server
     */
    webUrl(imagePath, relativeToWebroot = null) {
        let relativePath;
        
        if (relativeToWebroot) {
            relativePath = path.relative(relativeToWebroot, imagePath);
        } else {
            relativePath = path.basename(imagePath);
        }
        
        return new URL(relativePath, this.baseUrl + '/').href;
    }

    /**
     * Get MIME type for image files
     */
    getMimeType(imagePath) {
        const ext = path.extname(imagePath).toLowerCase();
        const mimeTypes = {
            '.jpg': 'image/jpeg',
            '.jpeg': 'image/jpeg',
            '.png': 'image/png',
            '.gif': 'image/gif',
            '.bmp': 'image/bmp',
            '.webp': 'image/webp',
            '.svg': 'image/svg+xml'
        };
        return mimeTypes[ext] || 'image/jpeg';
    }

    /**
     * Start a local Express server to serve images
     */
    async startLocalServer(directory = '.', port = 3000) {
        const express = require('express');
        const app = express();
        
        // Serve static files from the specified directory
        app.use(express.static(directory));
        
        // API endpoint to get image URLs
        app.get('/api/image-urls/:filename', (req, res) => {
            const filename = req.params.filename;
            const imagePath = path.join(directory, filename);
            
            if (!fs.existsSync(imagePath)) {
                return res.status(404).json({ error: 'Image not found' });
            }
            
            try {
                const urls = this.generateAllUrls(imagePath);
                res.json(urls);
            } catch (error) {
                res.status(500).json({ error: error.message });
            }
        });
        
        return new Promise((resolve, reject) => {
            this.server = app.listen(port, 'localhost', () => {
                this.baseUrl = `http://localhost:${port}`;
                console.log(`Local server started at ${this.baseUrl}`);
                resolve(this.baseUrl);
            });
            
            this.server.on('error', reject);
        });
    }

    /**
     * Stop the local server
     */
    stopLocalServer() {
        if (this.server) {
            this.server.close(() => {
                console.log('Local server stopped');
            });
            this.server = null;
        }
    }

    /**
     * Generate all possible URL formats for an image
     */
    generateAllUrls(imagePath) {
        return {
            fileUrl: this.fileUrl(imagePath),
            dataUrl: this.dataUrl(imagePath),
            webUrl: this.webUrl(imagePath)
        };
    }

    /**
     * Create an HTML viewer for the image with all URL types
     */
    createHtmlViewer(imagePath, outputFile = 'image_viewer.html') {
        const urls = this.generateAllUrls(imagePath);
        const imageName = path.basename(imagePath);
        
        const htmlContent = `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Image Viewer - ${imageName}</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            background: white;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
            backdrop-filter: blur(10px);
        }
        .header {
            text-align: center;
            margin-bottom: 30px;
        }
        .header h1 {
            color: #333;
            margin-bottom: 10px;
        }
        .image-display {
            text-align: center;
            margin: 30px 0;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 10px;
        }
        .image-display img {
            max-width: 100%;
            max-height: 400px;
            border-radius: 10px;
            box-shadow: 0 8px 16px rgba(0,0,0,0.2);
            transition: transform 0.3s ease;
        }
        .image-display img:hover {
            transform: scale(1.05);
        }
        .url-section {
            margin: 20px 0;
            padding: 20px;
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            border-radius: 10px;
            border-left: 5px solid #007bff;
            transition: all 0.3s ease;
        }
        .url-section:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        .url-type {
            font-weight: bold;
            color: #007bff;
            margin-bottom: 10px;
            font-size: 16px;
        }
        .url-description {
            color: #666;
            font-size: 14px;
            margin-bottom: 10px;
        }
        .url-text {
            font-family: 'Courier New', monospace;
            background: #fff;
            padding: 12px;
            border-radius: 5px;
            word-break: break-all;
            font-size: 12px;
            border: 1px solid #e9ecef;
            max-height: 100px;
            overflow-y: auto;
        }
        .copy-btn {
            background: linear-gradient(135deg, #007bff 0%, #0056b3 100%);
            color: white;
            border: none;
            padding: 8px 15px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 12px;
            margin-top: 10px;
            transition: all 0.3s ease;
        }
        .copy-btn:hover {
            transform: translateY(-1px);
            box-shadow: 0 3px 10px rgba(0,123,255,0.3);
        }
        .success-message {
            color: #28a745;
            font-size: 12px;
            margin-top: 5px;
            opacity: 0;
            transition: opacity 0.3s ease;
        }
        .success-message.show {
            opacity: 1;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🖼️ Image Viewer</h1>
            <p><strong>${imageName}</strong></p>
        </div>
        
        <div class="image-display">
            <img src="${urls.dataUrl}" alt="${imageName}" loading="lazy">
        </div>
        
        <div class="url-section">
            <div class="url-type">📁 File URL</div>
            <div class="url-description">Direct filesystem access (works locally)</div>
            <div class="url-text">${urls.fileUrl}</div>
            <button class="copy-btn" onclick="copyToClipboard('${urls.fileUrl.replace(/'/g, "\\'")}', this)">Copy URL</button>
            <div class="success-message">Copied to clipboard!</div>
        </div>
        
        <div class="url-section">
            <div class="url-type">🌐 Web URL</div>
            <div class="url-description">HTTP server access (shareable link)</div>
            <div class="url-text">${urls.webUrl}</div>
            <button class="copy-btn" onclick="copyToClipboard('${urls.webUrl}', this)">Copy URL</button>
            <div class="success-message">Copied to clipboard!</div>
        </div>
        
        <div class="url-section">
            <div class="url-type">📊 Data URL</div>
            <div class="url-description">Base64 encoded (embedded, works anywhere)</div>
            <div class="url-text">${urls.dataUrl.substring(0, 100)}...</div>
            <button class="copy-btn" onclick="copyToClipboard('${urls.dataUrl.replace(/'/g, "\\'")}', this)">Copy Full Data URL</button>
            <div class="success-message">Copied to clipboard!</div>
        </div>
    </div>
    
    <script>
        async function copyToClipboard(text, button) {
            try {
                await navigator.clipboard.writeText(text);
                const successMsg = button.nextElementSibling;
                successMsg.classList.add('show');
                setTimeout(() => {
                    successMsg.classList.remove('show');
                }, 2000);
            } catch (err) {
                console.error('Could not copy text: ', err);
                alert('Failed to copy to clipboard');
            }
        }
        
        // Add some interactivity
        document.addEventListener('DOMContentLoaded', function() {
            const urlSections = document.querySelectorAll('.url-section');
            urlSections.forEach((section, index) => {
                section.style.animationDelay = index * 0.1 + 's';
            });
        });
    </script>
</body>
</html>`;
        
        fs.writeFileSync(outputFile, htmlContent);
        return path.resolve(outputFile);
    }
}

/**
 * Browser-compatible version for client-side usage
 */
class BrowserImageURLGenerator {
    constructor(baseUrl = window.location.origin) {
        this.baseUrl = baseUrl;
    }

    /**
     * Generate a blob URL for File objects
     */
    blobUrl(file) {
        return URL.createObjectURL(file);
    }

    /**
     * Generate data URL from File object
     */
    async dataUrlFromFile(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = () => resolve(reader.result);
            reader.onerror = reject;
            reader.readAsDataURL(file);
        });
    }

    /**
     * Generate web URL for uploaded files
     */
    webUrl(filename) {
        return new URL(`/uploads/${filename}`, this.baseUrl).href;
    }

    /**
     * Create image preview with URLs
     */
    async createImagePreview(file, container) {
        const dataUrl = await this.dataUrlFromFile(file);
        const blobUrl = this.blobUrl(file);
        
        const previewHtml = `
            <div class="image-preview">
                <img src="${dataUrl}" alt="${file.name}" style="max-width: 300px; max-height: 200px;">
                <div class="url-info">
                    <p><strong>Blob URL:</strong> <code>${blobUrl}</code></p>
                    <p><strong>Data URL:</strong> <code>${dataUrl.substring(0, 50)}...</code></p>
                </div>
            </div>
        `;
        
        if (typeof container === 'string') {
            document.getElementById(container).innerHTML = previewHtml;
        } else {
            container.innerHTML = previewHtml;
        }
        
        return { dataUrl, blobUrl };
    }
}

// Demo function for Node.js
async function demo() {
    const generator = new ImageURLGenerator();
    
    console.log('Image URL Generator Demo (JavaScript)');
    console.log('=' .repeat(40));
    
    // For demo, we'll assume there's an image file
    const imagePath = process.argv[2] || 'demo_image.jpg';
    
    if (!fs.existsSync(imagePath)) {
        console.log(`Error: Image not found at ${imagePath}`);
        console.log('Please provide an image path as an argument.');
        return;
    }
    
    console.log(`Generating URLs for: ${imagePath}`);
    console.log('-'.repeat(40));
    
    try {
        const urls = generator.generateAllUrls(imagePath);
        
        console.log(`File URL: ${urls.fileUrl}`);
        console.log(`Web URL: ${urls.webUrl}`);
        console.log(`Data URL: ${urls.dataUrl.substring(0, 80)}...`);
        
        // Create HTML viewer
        const htmlFile = generator.createHtmlViewer(imagePath);
        console.log(`Created HTML viewer: ${htmlFile}`);
        
        // Start server
        const serverUrl = await generator.startLocalServer();
        console.log(`Server started at: ${serverUrl}`);
        console.log(`View image at: ${serverUrl}/image_viewer.html`);
        
        // Keep server running
        process.on('SIGINT', () => {
            generator.stopLocalServer();
            process.exit(0);
        });
        
    } catch (error) {
        console.error(`Error: ${error.message}`);
    }
}

// Export for both Node.js and browser environments
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { ImageURLGenerator, BrowserImageURLGenerator };
    
    // Run demo if this file is executed directly
    if (require.main === module) {
        demo();
    }
} else {
    // Browser environment
    window.ImageURLGenerator = BrowserImageURLGenerator;
}