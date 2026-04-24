# Project Portfolio — Umama

> Second-year Computer Science student at FAST-NUCES with a growing focus on **human-centered computing** — specifically how interface structure, interaction design, and system architecture affect the way people process information and learn.

These projects reflect a trajectory: from low-level systems programming toward research in adaptive and accessible learning technologies. My current work sits at the intersection of **interaction design**, **cognitive accessibility**, and **immersive learning environments**.

---

## NeuroLearn AI — Adaptive Learning Platform for Neurodivergent Students
**`HTML` · `CSS` · `JavaScript` · `UX Research` · `Accessible Design`**

> Live at **[neurolearnai.netlify.app](https://neurolearnai.netlify.app)**

A deployed web-based learning platform designed to support neurodivergent learners through structured, low-friction interfaces and adaptive content delivery. Built around the principle that interface design functions as an accessibility intervention — not an afterthought.

**The research question driving this project:**
*Does modular, chunked content delivery with consistent spatial layout reduce task-switching errors and improve task completion rates for neurodivergent learners?*

**What it does**
- Learning preference onboarding — Visual, Auditory, Kinesthetic, Reading/Writing
- Adaptive dashboard with personalized content sequencing and progress tracking
- Course catalog designed around neurodivergent-specific learning patterns (ADHD, dyslexia, autism spectrum)
- Analytics section with learning style breakdown and behavioral insights
- Accessible UI designed to WCAG 2.1 AA standards — consistent layout, chunked content, reduced cognitive load

**Design principles**
- Spatial consistency — predictable layout across all sessions reduces orientation overhead
- Content chunking — information delivered in discrete units rather than continuous streams
- Low-distraction interface — intentional minimalism to reduce extraneous cognitive load
- Personalization — content flow adapts to declared learning modality

**Tech stack**
`HTML5` · `CSS3` · `Vanilla JavaScript` · `Responsive Design` · `Accessibility-first UI`

**Status:** Deployed and live. Backend adaptive logic in active development.

---

## Academic Productivity Toolkit
**`C++` · `Qt Framework` · `Desktop` · `Event-Driven UI`**

A cross-platform desktop application that helps students manage deadlines, tasks, and weekly schedules — built around Qt's signal-slot architecture.

**Why it matters beyond the code:**
This project started from a specific observation — students under cognitive load need interfaces that reduce friction, not amplify it. Every design decision (task sequencing, visual hierarchy, state feedback) was made with that constraint in mind. That thinking now directly informs my research interest in how interface structure affects learning for neurodivergent users.

**What it does**
- Task and deadline manager with pending/completed state tracking
- Calendar view for daily and weekly planning
- Progress tracker that visualizes goal completion over time
- Qt Widgets UI with clean signal-slot event handling

**Tech stack**
`C++` · `Qt Framework` · `Signal-slot mechanism` · `OOP design principles`

---

## Smart Banking Wallet System
**`C++` · `OOP` · `Custom DSA` · `File I/O`**

A fully-featured, console-based banking system engineered without STL containers — every data structure built from scratch to understand the underlying mechanics.

**What it does**

| Feature | Description |
|---|---|
| Secure login | Password + 4-digit PIN auth, lockout after 3 failed attempts, timed auto-unlock |
| Transactions | Deposit, withdraw, transfer with real-time balance updates |
| Undo system | Reverse any transaction within a 60-second window via custom stack |
| Scheduled payments | Min-heap priority queue for future payments ordered by execution time |
| Security hardening | Weak credential rejection, per-account audit logs, invalid login tracking |
| Persistence | All state survives restarts via structured file I/O |

**Data structures — hand-built**
```
TransactionLinkedList   →  full history per account, O(1) append
TransactionStack        →  undo buffer with timestamp-gated rollback
PaymentPriorityQueue    →  min-heap ordered by execution time
Dynamic User Array      →  auto-resizing account storage
```

**Tech stack**
`C++` · `OOP` · `Manual memory management` · `File I/O` · `Input validation`

---

## E-Voting System
**`C` · `Procedural` · `Console`**

A secure voting simulator covering the full election lifecycle — registration, session management, and result announcement — across two distinct user roles.

**What it does**
- Admin role: password-protected login, session control, live result monitoring
- Voter role: registration with auto-generated ID, single-vote enforcement
- Result engine with real-time tallying and winner announcement
- Modular architecture — clean separation of concerns, easy to audit and extend

**Tech stack**
`C` · `Modular functions` · `Switch-case control flow` · `Input validation`

---

## Book Recommendation Website
**`HTML5` · `CSS3` · `JavaScript`**

A responsive, genre-aware book discovery site built without any frameworks — pure HTML, CSS, and vanilla JavaScript.

**What it does**
- Genre filtering with dynamic DOM updates
- Mobile-first responsive layout using Flexbox and Grid
- Smooth interactions in vanilla JavaScript
- Intentional typography and spacing for readability

**Tech stack**
`HTML5` · `CSS3 (Flexbox, Grid)` · `Vanilla JavaScript`

---

## Technical Profile

| Area | Tools & Languages |
|---|---|
| Systems programming | C, C++ — manual memory management, custom data structures |
| Desktop applications | Qt Framework, OOP architecture, event-driven design |
| Web & UI | HTML5, CSS3, Vanilla JavaScript, responsive design |
| Research interests | Human-computer interaction, cognitive accessibility, adaptive learning systems, XR/immersive environments |
| Concepts | Cognitive load reduction, interface-as-accessibility-intervention, neurodivergent UX, spatial computing |

---

## Research Direction

My current academic interest lies at the intersection of **interactive systems**, **XR technologies**, and **cognitive learning environments**. I am less focused on building complete applications and more focused on understanding the principles that govern how users interact with information across different digital contexts — and how those systems can be improved for clarity, accessibility, and adaptability.

I am particularly interested in whether immersive and spatially-structured environments can reduce cognitive overload and improve learning outcomes for neurodivergent users — a question I am beginning to explore through both design and emerging research.

---

*Built with curiosity, a lot of compiler errors, and a genuine interest in how software shapes human experience.*
