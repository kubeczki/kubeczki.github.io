# Project principles

## Engineering philosophy

- Follow data-oriented design. When making architectural or performance decisions, prefer the principles taught by Mike Acton.
- Keep everything simple, direct, and explicit.
- Prefer no code over code. Prefer a small native solution over adding a library.
- Do not introduce frameworks, libraries, build systems, abstractions, or tooling unless they solve a demonstrated need that cannot be handled simply.
- Treat code as a liability. Remove unnecessary code and avoid clever, speculative, or overly generic solutions.
- Build only what the current problem requires.

## User experience

- The user comes first.
- Performance is a core feature. This static site should load as quickly as reasonably possible and must not feel laggy.
- Minimize HTML, CSS, JavaScript, assets, requests, and transferred bytes.
- Prefer HTML and CSS. Add JavaScript only when the required behavior cannot be achieved without it.
- Avoid client-side frameworks and runtime dependencies.
- Prevent layout shifts, blocking resources, unnecessary animation, and expensive work on the main thread.
- Optimize images and other assets for their actual display size and format.

## Responsive design

- The site must look excellent and work correctly on both phones and desktop computers.
- Design mobile-first, then enhance the layout for larger screens.
- Test narrow and wide viewports after meaningful visual changes.
- Keep content readable, controls usable by touch, and layouts robust across intermediate screen sizes.

## Decision rule

When choosing between solutions, prefer the one with fewer moving parts, less code, less runtime work, and a better experience for the visitor.
