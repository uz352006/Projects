# Projects

Welcome to my project portfolio! This repository showcases my key development projects, demonstrating my experience with **C**, **C++ (Qt)**, **C++ (OOP/DSA)**, and **HTML/CSS/JS**. Each project reflects my growing skills in programming, problem-solving, and user-centric design.

---

## 🏦 Smart Banking Wallet System (C++ / OOP / DSA)

A fully featured, console-based banking system built from scratch using custom data structures. The system manages user accounts, transactions, scheduled payments, and security — all without using STL containers.

### 🔧 Features:
- **Account management** — create accounts, secure login, account lockout after failed attempts, profile updates
- **Transactions** — deposit, withdraw, fund transfer with PIN verification and full transaction history
- **Undo system** — 60-second undo window using a custom stack; supports deposit, withdraw, and transfer reversal
- **Scheduled payments** — priority queue–based scheduler; set payments minutes, hours, days, or months ahead; cancel anytime
- **Security** — weak password/PIN detection, 3-attempt lockout, timed unlock, security audit log per account
- **Persistence** — all user data, transaction history, undo stacks, and scheduled payments saved to file automatically

### 🗂️ Data Structures (implemented from scratch, no STL):
- **Linked list** — transaction history per user
- **Stack** — undo system with time-limited rollback
- **Priority queue** — scheduled payment execution ordered by time
- **Dynamic array** — resizable user storage

### 👨‍💻 Tech Stack:
- C++ (OOP)
- Custom DSA (no STL containers)
- File I/O for persistence
- Input validation & security logic

---

## 🗳️ E-Voting System (C Language)
...
