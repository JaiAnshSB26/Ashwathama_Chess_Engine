import { useState } from 'react'
import './App.css'

function App() {
  return (
    <div className="min-h-screen flex items-center justify-center p-8">
      <div className="glass-effect rounded-2xl p-8 max-w-4xl w-full">
        <h1 className="text-5xl font-bold bg-gradient-to-r from-purple-400 via-pink-500 to-blue-500 bg-clip-text text-transparent mb-4">
          Ashwathama Chess Engine
        </h1>
        <p className="text-gray-300 text-lg mb-8">
          Professional UCI Chess Engine with Modern Web Interface
        </p>
        
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="bg-white/5 rounded-lg p-6 hover:bg-white/10 transition-all">
            <h3 className="text-xl font-semibold mb-2">🎮 Interactive Board</h3>
            <p className="text-sm text-gray-400">Drag and drop pieces with smooth animations</p>
          </div>
          
          <div className="bg-white/5 rounded-lg p-6 hover:bg-white/10 transition-all">
            <h3 className="text-xl font-semibold mb-2">🧠 Alpha-Beta Search</h3>
            <p className="text-sm text-gray-400">Advanced position evaluation</p>
          </div>
          
          <div className="bg-white/5 rounded-lg p-6 hover:bg-white/10 transition-all">
            <h3 className="text-xl font-semibold mb-2">📊 Move Analysis</h3>
            <p className="text-sm text-gray-400">Real-time position evaluation</p>
          </div>
        </div>

        <div className="mt-8 text-center">
          <p className="text-sm text-gray-400">
            Building the full chess UI... 🚀
          </p>
        </div>
      </div>
    </div>
  )
}

export default App

