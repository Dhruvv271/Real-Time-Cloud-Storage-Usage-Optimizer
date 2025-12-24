# Cloud Storage Usage Optimizer
Have to fix the Dashboard file for "Getting Recommendations" on the webpage, though its working on the terminal when you run the server. 
Work in progress
## MVP Scope

The Minimum Viable Product (MVP) focuses on:
- Simulating cloud storage events
- Real-time processing of storage usage metrics
- Classification of files as hot or cold based on access patterns
- Basic optimization recommendations
- A simple dashboard for visualization

Advanced features such as multi-cloud support, machine learning,
and distributed deployment are intentionally excluded from the MVP.
## Core Components
- Event Simulator (C++)
- Real-time Analytics Engine (C++)
## Optimization Strategy
- Files accessed fewer than 3 times are classified as cold
- Cold files are recommended for archival storage
- Cost savings are estimated using tiered storage pricing
## HTTP API Server
- GET /hello – Health check.
- POST /event – Submit storage events in JSON format.
- GET /analytics – Retrieve file analytics as JSON.
- GET /recommendations – Retrieve optimization recommendations as JSON.
- GET /shutdown – Gracefully stop the server and worker threads.
- Supports CORS for frontend access.
## Web dashboard
- Submit events via a form.
- View analytics and recommendations dynamically.
- Auto-refreshes every 5 seconds.
## Logging 
- Records server start, events received, worker thread lifecycle, and errors.