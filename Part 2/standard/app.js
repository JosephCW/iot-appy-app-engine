/**
 * Copyright 2017, Google, Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

'use strict';

// [START gae_node_request_example]
const express = require('express');

const app = express();

let timesHelloed = 0;
let heartRates = [];
let min = 0;
let max = 0;
let mean = 0;
let median = 0;
let mode = 0;

app.get('/', (req, res) => {
  res
    .status(200)
    .send('Hello, world!')
    .end();
});

app.get('/addData', (req, res) => {
  let newHeartRate = parseInt(req.query.heartRate);
  heartRates.push(newHeartRate);

  // calculate minimum
  if (heartRates.length == 1) {
    min = newHeartRate;
    max = newHeartRate;
    mean = newHeartRate;
    median = newHeartRate;
    mode = newHeartRate;
  } else {
    // calculate mean/median/mode
    // mean is just sum/amount
    mean = heartRates.reduce((previous, current) => current += previous) / heartRates.length;
    // median sort and then get middle index
    heartRates.sort();
    min = heartRates[0];
    max = heartRates[heartRates.length - 1];
    median = 0;
    if (heartRates.length % 2 == 0) {
      median = (heartRates[heartRates.length/2] + heartRates[(heartRates.length/2) - 1]) / 2
    } else {
      median = heartRates[Math.floor(heartRates.length/2)]
    }
    // mode do a for loop w/ dictionary to calculate
    // should be kept outside of restful call but w/e
    let appearances = {}
    for (let i = 0; i < heartRates.length; i++) {
      appearances[heartRates[i]] = (appearances[heartRates[i]] || 0) + 1;
    }
    // iterate through the dictionary to find out the key
    // that has highest value
    let highest_appearing_key = -1;
    let highest_appearing_key_count = -1;
    for (let key in appearances) {
      if (appearances[key] > highest_appearing_key_count) {
        highest_appearing_key = key;
        highest_appearing_key_count = appearances[key];
      }
    }
    mode = highest_appearing_key
  }

  res
    .status(200)
    .send('Thanks for adding data :)')
    .end();
});

app.get('/statistics', (req, res) => {
  res
    .status(200)
    .send(`mean: ${mean}
    median: ${median}
    mode: ${mode}
    min: ${min}
    max: ${max}`)
    .end();
})

// Start the server
const PORT = process.env.PORT || 8080;
app.listen(PORT, () => {
  console.log(`App listening on port ${PORT}`);
  console.log('Press Ctrl+C to quit.');
});
// [END gae_node_request_example]

module.exports = app;
