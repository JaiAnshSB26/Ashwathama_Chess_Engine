import ChessBoardUI from "./components/ChessBoardUI.jsx";
import "./App.css";

export default function App() {
  return (
    <div
      className="
        min-h-screen
        text-white
        flex flex-col items-center
        overflow-x-hidden
        overflow-y-auto
        pb-16 pt-10
        bg-gradient-to-b
        from-[#050509] via-[#0a1220] to-[#001b24]
      "
    >
      {/* HEADER */}
      <div
        className="
          w-full
          max-w-[1400px]
          px-6
          flex flex-col items-center
          text-center
          mb-8
        "
      >
        <div
          className="
            w-full
            max-w-[1500px]
            rounded-xl
            border border-white/10
            bg-[rgba(0,0,0,0.4)]
            backdrop-blur-xl
            px-6 py-6
            shadow-[0_0_40px_rgba(0,255,180,0.08)]
          "
        >
          <h1
            className="
              text-4xl font-bold
              bg-gradient-to-r from-[#ff4f7a] via-[#ff7a95] to-[#ff4f7a]
              bg-clip-text text-transparent
              drop-shadow-[0_0_15px_rgba(255,80,120,0.5)]
            "
          >
            Ashwathama Chess Engine
          </h1>

          <p className="text-slate-300 text-sm mt-3">
            Version 1.0 of an attempted Professional UCI Chess Engine with Modern Web Interface, built from scratch in C++ by 5 students, hosted using UCI, React, Flask and other tools.
            (Note: We are aware that there might be some hiccups due to some unresolved edge case handling right now, we are working on fixing that, and versions 1.1 and 2.0 would be free of these occasional hiccups.
            For now, if you encounter an unexpected engine error on a legal move, please bear with us and start a new game, if the problem persists, please refresh the page and reach out to us. (Developer Contact: jaiansh2005@gmail.com)
            Important: This is a student project and not affiliated with any professional chess organization.)
          </p>
        </div>
      </div>

      {/* MAIN PLAY AREA */}
      <div
        className="
          w-full
          flex
          justify-center
          px-6
        "
      >
        <div
          className="
            play-shell
            w-full
            max-w-[1150px]
            p-8
            flex
            justify-center
          "
        >
          {/* We keep ChessBoardUI exactly as you built it (board + sidebar). */}
          <ChessBoardUI />
        </div>
      </div>

      {/* FOOTER NOTE */}
      <div className="mt-10 text-[10px] text-slate-600 text-center">
        © Ashwathama Engine — 2025 (built with passion and dedication by 5 students at Bachelor of Science, Ecoole Polytechnique, Palaiseau.)
      </div>
    </div>
  );
}
