/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./App.css";

import React, { useState } from "react";
import { BrowserRouter as Router, Link, Route, Routes } from "react-router-dom";

import BlankPage from "./components/BlankPage";
import HomePage from "./components/HomePage";

/*
 * Main app definition
 *
 * The app consists of a navigation bar and a router. The home page is
 * the main game page, and the blank page is a placeholder for future
 * pages.
 */
function App() {
  const [isNavbarVisible, setIsNavbarVisible] = useState(true);

  return (
    <Router>
      <div>
        {isNavbarVisible ? (
          <nav>
            <ul>
              <li>
                <Link to="/">Home</Link>
              </li>
              <li>
                <Link to="/blank">Blank</Link>
              </li>
              <li>
                <button
                  className="hideNavbarButton"
                  onClick={() => setIsNavbarVisible(false)}
                  aria-label="Hide Menu"
                >
                  {/*
                   * Show an up arrow in the shape of a caret to hide the navbar menu
                   *
                   * Explanation of SVG path:
                   *
                   *   M512 288: Move to the starting point (512, 288).
                   *   L160 640: Draw a line from the starting point to (160, 640).
                   *   L192 640: Extend the bottom left of the arrow to (192, 640).
                   *   L192 736: Draw a line to (192, 736).
                   *   L832 736: Draw a line to (832, 736).
                   *   L832 640: Extend the bottom right of the arrow to (832, 640).
                   *   L864 640Z: Draw a line to (864, 640) and close the path.
                   */}
                  <svg
                    viewBox="0 0 1024 1024"
                    width="32"
                    height="32"
                    fill="#ffffff"
                  >
                    <path d="M512 288L160 640L192 640L192 736L832 736L832 640L864 640Z" />
                  </svg>
                </button>
              </li>
            </ul>
          </nav>
        ) : (
          <button
            className="showNavbarButton"
            onClick={() => setIsNavbarVisible(true)}
            aria-label="Show Menu"
          >
            {/*
             * Show a hamburger icon to bring back the navbar menu
             */}
            <svg viewBox="0 0 32 32" width="32" height="32">
              <rect
                x="0"
                y="4"
                width="32"
                height="4"
                fill="rgba(255, 255, 255, 0.5)"
              />
              <rect
                x="0"
                y="14"
                width="32"
                height="4"
                fill="rgba(255, 255, 255, 0.5)"
              />
              <rect
                x="0"
                y="24"
                width="32"
                height="4"
                fill="rgba(255, 255, 255, 0.5)"
              />
            </svg>
          </button>
        )}

        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/blank" element={<BlankPage />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;
