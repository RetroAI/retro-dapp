/*
 * Copyright (C) 2023-2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

import "./index.css";

import React from "react";
import ReactDOM from "react-dom/client";

import App from "./App";

/*
 * Vite entry point
 */
const root = document.getElementById("root");
if (root) {
  // Obtain a root instance from ReactDOM using the createRoot method
  const appRoot = ReactDOM.createRoot(root);

  // Render the App component within the root instance
  appRoot.render(
    // Using React's StrictMode to highlight potential problems in an app
    <React.StrictMode>
      <App />
    </React.StrictMode>,
  );
}
