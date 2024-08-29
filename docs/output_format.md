### Data Format

The sample metrics JSON object has the following format (annotated using JSON5 comments). 
The sample was abridged and simplified for clarity (read comments). Please note that the keys are not guaranteed to be
ordered.

```json5
{
  "error": "", // error string, if any
  "version": "3.10.1", // Physiology version
  "pulse": { // pulse metrics
    "hr":{ // heart rate/pulse  
      "10":{ // key: seconds (not necessarily in order)
        "value": 58.9, // beats per minute (bpm)
        "confidence": 0.95 // confidence score based on signal-to-noise ratio
      },
      "11":{
        "value": 58.2,
        "confidence": 0.94
      },
      "12":{
        "value": 58.1,
        "confidence": 0.91
      } // will continue to span the duration of the measurement
    },
    "hr_trace": { // pulse trace
      // keys are not necessarily in order
      "0":{ "value": 0.5}, // key: exact time, in seconds, of frame capture from beginning of current measurement  
      "0.033":{ "value": 0.56}, // value: est. blood throughput at the time in the key
      "0.066":{ "value": 0.59} 
      // since typical video framerate is 30 fps, expect a much longer array than "hr", spanning the whole measurement
    },
    "hr_strict":{ // aggregate average heart rate (usually, a single value) 
      "28":{ "confidence": [ 32.4054 ], "value": 60.5}  
    },
    "hrv":{} // heart rate variability: not yet available to the public
  },
  "breath": { // breath metrics
    "rr":{ // breath rate
      "15":{ // key: seconds
        "value": 18.9, // value: breath rate in breaths per minute (Bpm)
        "confidence": 0.95 // confidence score based on signal-to-noise ratio
      },
      "16":{
        "value": 18.2,
        "confidence": 0.94
      },
      "17":{
        "value": 18.1,
        "confidence": 0.91
      } // will continue to span the duration of the measurement
    },
    "rr_trace": { // upper breath trace, similar to pulse trace, i.e. "hr_trace"
      // "upper" refers signal from upper torso/chest area, may be different from "rr_trace_lower"
      "0":{ "value": 0.5}, // value: amount of visual chest expansion
      "0.033":{ "value": 0.56},
      "0.066":{ "value": 0.59},
      // expect much longer array than "rr", 
      // values for each processed frame, not second, spanning the whole measurement
    },
    "rr_trace_lower": {
      // "lower" refers to signal from lower torso/abdomen area
      // e.g. may help to determine if the breathing is shallow or deep
      "0":{ "value": 0.5}, // value: amount of visual abdomen expansion
      "0.033":{ "value": 0.56},
      "0.066":{ "value": 0.59},
      // values for each processed frame, not second, spanning the whole measurement
    },
    "rrl":{"0":{ "value": 0.5}}, //respiratory line length
    "apnea":{"0":{ "value": false}},  // apnea detected or not, keyed by specific time points (in seconds)
    //  I:E ratio denotes the proportions of each breath cycle 
    //  devoted to the inspiratory and expiratory phases
    "ie":{"0":{ "value": 1.5}}, //  keyed by specific time points (in seconds)
    "amplitude":{"0":{ "value": 0.5}}, // amplitude of the upper breath trace at specific time points (in seconds)
    "baseline":{"0":{ "value": 0.5}}  
  },
  "pressure": {
    "phasic":{"0":{ "value": 0.5}}, // phasic pressure, keyed by specific time points (in seconds) - not currently available to the public
  },
  "face": {
    // face states are keyed by specific time points (in seconds) and are detected for each frame
    "blinking":{"0":{ "value": false}},
    "talking":{"0":{ "value": false}},
  }
}
```
