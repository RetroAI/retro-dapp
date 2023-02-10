/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See the file Apache-2.0.txt for more information.
 */

import "./index.css";

import React from "react";
import ReactDOM from "react-dom";

import App from "./App";

/*
 * Vite entry point
 */
ReactDOM.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
  document.getElementById("root"),
);
