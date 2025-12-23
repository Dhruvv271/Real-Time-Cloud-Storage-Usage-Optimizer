# Cloud Storage Usage Optimizer
Work in progress.
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
