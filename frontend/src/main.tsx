/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See the file Apache-2.0.txt for more information.
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
