# Ashwathama Chess Engine

Welcome to the public repository for the Ashwathama Chess Engine, a chess Engine developed in C++ from scratch by a team of 4-5 students from Ecole Polytechnique Bachelor of Science program.

A two-phase project blending high-performance C++ engine development with a modern full-stack deployment pipeline (Flask + React + Nginx + GCP + Vercel).  
Originally built as a course project at École Polytechnique and later extended into a production-grade online playable engine with full UCI integration and TLS-secured APIs.

---
## Phase 1 — Engine Development (C++ Team Project)

**Team members:**  
Jai Ansh Singh Bindra · Kingshuk Gupta · Mohamed Iyad Boualem · Ziji Wang · Tomas Gradowski  

**Language & Paradigm:** C++ · Bitboards · Object-Oriented Design · UCI Protocol  
**Duration:** 7 weeks (University project)

### Overview
The first phase focused on **building a fully functional chess engine from the ground up** without using any external libraries for core chess logic.  
The engine was designed to be modular, efficient, and compliant with the **UCI (Universal Chess Interface)** (added later in the form of `uci.hpp` and `uci.cpp` files in the src in the 2nd phase of the project), enabling it to communicate seamlessly with front-end GUIs or other engines.

### Core Architecture
- **Board Representation:** Implemented with **64-bit bitboards**, allowing fast bitwise operations for move generation and legality checks.  
- **Move Generation:** Built a hierarchical system handling all piece types, castling, en passant, and promotions; validated each generated move for legality via simulated board states.  
- **Search Algorithm:** Implemented an **iterative-deepening minimax search** with **alpha-beta pruning** for efficient exploration of the move tree.  
- **Evaluation Function:** Combined **material balance**, **piece-square tables**, and **mobility heuristics** into a linear evaluation formula producing centipawn scores.  
- **Move Ordering & Heuristics:** Added basic move ordering (captures first, killer moves) to improve pruning effectiveness.  
- **Game State Management:** Supported **FEN parsing**, **move history**, and **undo/redo** for debugging and analysis.  
- **Search Depth Control:** Adjustable depth (plies) and time-control limits for fair benchmarking and self-play.  
- **Referee Integration:** Built standard I/O wrappers (CLI) to interact with provided referee systems during testing and tournaments.

### Key Outcomes
- Achieved a **stable and complete chess engine** capable of legal move generation, evaluation, and full game play through UCI commands.  
- Passed all referee-defined test cases, including **checkmate detection, stalemate recognition, and draw conditions**.  
- Successfully played engine-vs-engine matches and self-play loops to verify search correctness.  
- Provided a clear modular codebase ready for integration with external systems or GUIs — which enabled the later web deployment.

### Results Obtained:-
We consistently ranked in Top 12 teams out of the 25-26 total teams competing in the class championship for most parts of the competition from Week 1 to Week 7. We finished the competition with a Top 12 out of 25 - 26 which was a great achievement for us since this was our first major project entirely in C++ and we were able to compete with teams having members with years of experience in C++!
---

## Phase 2 — Deployment & Infrastructure (Post-course Extension by Jai Ansh Bindra)

**Goal:** Transform the engine into a publicly playable AI web app with a clean frontend, robust backend, and secure cloud deployment.

### Architecture Overview

### Backend (Flask + Gunicorn + Nginx on GCP)
- Wrapped the compiled engine in a Python Flask API (`server.py`, `engine_runner.py`).  
- Exposed `/move` and `/health` endpoints with JSON payloads.  
- Configured CORS to allow only `https://www.ashwathama-chess.com`.  
- Deployed on a Google Cloud e2-micro Ubuntu instance using Gunicorn and Systemd.  
- Added reverse proxy with Nginx for TLS termination and request routing.  
- Secured the API with Certbot SSL certificates (auto-renew timer verified).

### Frontend (React + Vite + Vercel)
- Built a responsive chessboard UI (`ChessBoardUI.jsx`) with move history and evaluation display.  
- Used React hooks and local state management for seamless move flow and game status.  
- Deployed to Vercel, connected to the backend via environment variable `VITE_BACKEND_URL`.  
- Configured DNS records on Namecheap and Vercel for the custom domain:
  - `ashwathama-chess.com` (frontend)
  - `api.ashwathama-chess.com` (backend)

### Domain & DNS
| Record | Host | Value | Purpose |
|--------|------|--------|----------|
| A | @ | 76.76.21.21 | Vercel frontend |
| CNAME | www | cname.vercel-dns.com. | Vercel subdomain |
| A | api | (GCP static IP) | Backend API endpoint |

### Challenges Solved
- CORS policy conflicts between Vercel and GCP origins.  
- Engine communication timeouts (deadline handling adjusted).  
- Pawn promotion edge-case crashes (identified and patched in local builds).  
- DNS propagation delays and Certbot SSL renewal management.

### Deployment Summary
| Layer | Stack | Provider |
|-------|--------|-----------|
| Engine | C++ (ELF binary) | GCP VM |
| Backend | Flask + Gunicorn + Nginx | GCP e2-micro Ubuntu |
| Frontend | React (Vite) | Vercel |
| DNS / TLS | Namecheap + Certbot | — |

**Live Demo:** [https://www.ashwathama-chess.com/](https://www.ashwathama-chess.com/)  
**Repository:** [https://github.com/JaiAnshSB26/Ashwathama_Chess_Engine](https://github.com/JaiAnshSB26/Ashwathama_Chess_Engine)

---

## Next Steps / Version 2 Roadmap
- Implement improved promotion UX and backend safeguards.  
- Add engine strength selector (Elo approximation via UCI depth).  
- Improve evaluation display and include real-time search logs.  
- Containerize deployment (Docker / Kubernetes).  
- Benchmark latency and optimize Flask async handling.

---

## Tech Stack
**Languages:** C++, Python, JavaScript (React)  
**Frameworks & Tools:** Flask, Gunicorn, Nginx, React (Vite), Certbot, GCP, Vercel, Namecheap DNS  
**Concepts:** Bitboards, UCI Protocol, Cloud Deployment, CORS, Reverse Proxy, TLS, Systemd Services  

---

## Authors

**Engine Development (Phase 1):**  
- Jai Singh Ansh Bindra  
- Kingshuk Gupta  
- Mohamed Iyad Boualem  
- Tomas Gradowski  
- Ziji Wang  

**Deployment & Infrastructure (Phase 2):**  
- Jai Ansh Singh Bindra (developer contact: jaiansh2005@gmail.com), along with the advice and support of all my lovely team members!

---

## License
MIT License — Open for educational and research use.




