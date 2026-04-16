# 🚀 Project Portfolio

> A collection of projects built across **C**, **C++**, and **Web** — each one tackling real-world problems through thoughtful design and solid engineering.

---

## 🏦 Smart Banking Wallet System
**`C++` · `OOP` · `Custom DSA` · `File I/O`**

> A fully-featured, console-based banking system engineered from scratch — no STL containers. Every data structure is hand-built, from the transaction linked list to the priority queue powering scheduled payments.

### ✨ What it does

| Feature | Description |
|---|---|
| 🔐 Secure login | Password + 4-digit PIN auth, account lockout after 3 failed attempts, timed auto-unlock |
| 💸 Transactions | Deposit, withdraw, and transfer funds with real-time balance updates |
| ↩️ Undo system | Reverse any transaction within a 60-second window — powered by a custom stack |
| ⏰ Scheduled payments | Queue future payments by minutes, hours, days, or months; cancel anytime |
| 🛡️ Security hardening | Weak password/PIN rejection, per-account security audit logs, invalid login tracking |
| 💾 Persistence | All data — accounts, history, undo stacks, scheduled payments — survives restarts via file I/O |

### 🗂️ Data structures (built from scratch)

```
TransactionLinkedList   →  full history per account, O(1) append
TransactionStack        →  undo buffer with timestamp-gated rollback
PaymentPriorityQueue    →  min-heap by execution time for scheduled transfers
Dynamic User Array      →  auto-resizing storage for all accounts
```

### 🛠️ Tech stack
`C++` · `Object-Oriented Programming` · `Manual memory management` · `File I/O` · `Input validation & security logic`

---

## 🗳️ E-Voting System
**`C` · `Procedural` · `Console`**

> A secure, console-based voting simulator covering the full election lifecycle — from voter registration to result announcement — across two distinct user roles.

### ✨ What it does

- **Admin role** — password-protected login, start/end voting sessions, view live results
- **Voter role** — registration with auto-generated voter ID, single-vote enforcement
- **Result engine** — real-time tallying with winner announcement
- **Clean architecture** — modular functions, no spaghetti; easy to read and extend

### 🛠️ Tech stack
`C` · `Switch-case control flow` · `Modular functions` · `Input validation`

---

## 📖 Book Recommendation Website
**`HTML5` · `CSS3` · `JavaScript`**

> A responsive, genre-aware book discovery website — built to feel good on any screen, from desktop to mobile.

### ✨ What it does

- **Genre filtering** — browse and narrow recommendations by category
- **Responsive layout** — Flexbox + Grid for fluid, mobile-first design
- **Interactive UI** — smooth DOM interactions without any frameworks
- **Clean aesthetics** — intentional typography and spacing for a pleasant reading experience

### 🛠️ Tech stack
`HTML5` · `CSS3 (Flexbox, Grid)` · `Vanilla JavaScript (DOM manipulation)`

---

## 📅 Academic Productivity App
**`C++` · `Qt Framework` · `Desktop`**

> A cross-platform desktop app that helps students stay on top of deadlines, tasks, and weekly schedules — built with Qt's event-driven UI model.

### ✨ What it does

- **Task & schedule manager** — create tasks, assign deadlines, track what's pending
- **Calendar view** — daily and weekly planning at a glance
- **Progress tracker** — visualize goal completion over time
- **Responsive UI** — Qt Widgets with signal-slot architecture for clean event handling

### 🛠️ Tech stack
`C++` · `Qt Framework` · `Signal-slot mechanism` · `OOP design principles`

---

<div align="center">

*Built with curiosity and a lot of compiler errors.*

</div>
