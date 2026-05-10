// Initialise Mermaid after each MkDocs Material instant-navigation page
// load. `document$` is the Material-provided RxJS subject that emits on
// every page transition; without re-running mermaid.initialize() the
// diagrams on subsequent pages would stay as raw text. Theme tracks the
// site colour scheme so diagrams adapt in dark mode.
(function () {
  if (typeof mermaid === "undefined") return;

  function currentTheme() {
    const palette = JSON.parse(localStorage.getItem("__palette") || "null");
    if (palette && typeof palette.index === "number") {
      return palette.index === 1 ? "dark" : "default";
    }
    return window.matchMedia("(prefers-color-scheme: dark)").matches
      ? "dark"
      : "default";
  }

  function init() {
    mermaid.initialize({
      startOnLoad: false,
      theme: currentTheme(),
      flowchart: { useMaxWidth: true, htmlLabels: true, curve: "basis" },
      sequence: { useMaxWidth: true, mirrorActors: false },
    });
    mermaid.run({ querySelector: ".mermaid" });
  }

  if (typeof document$ !== "undefined") {
    // Material for MkDocs (instant navigation enabled).
    document$.subscribe(init);
  } else if (document.readyState !== "loading") {
    init();
  } else {
    document.addEventListener("DOMContentLoaded", init);
  }
})();
