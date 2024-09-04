## Data Format


### Raw REST API Output Format 
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
    "hr_strict":  { // aggregate average heart rate (usually, a single value)
      // NOTE: will be an empty JSON object IFF signal-to-noise ratio is too low to compute this value
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
    "rr_strict":  { // aggregate average breath rate (usually, a single value)
      // NOTE: will be an empty JSON object IFF signal-to-noise ratio is too low to compute this value
      "28":{ "confidence": [ 32.4054 ], "value": 60.5}
    },
    "rrl":{"0":{ "value": 0.5}}, //respiratory line length
    "apnea":{"0":{ "value": false}},  // apnea detected or not, keyed by specific time points (in seconds)
    //  I:E ratio denotes the proportions of each breath cycle 
    //  devoted to the inspiratory and expiratory phases
    "ie":{"0":{ "value": 1.5}}, //  keyed by specific time points (in seconds)
    "amplitude":{"0":{ "value": 0.5}}, // amplitude of the upper breath trace at specific time points (in seconds)
    "baseline":{"0":{ "value": 0.5}}  
  },
  // Phasic blood pressure, keyed by specific time points (in seconds) 
  // NOTE: this is currently a crude approximation, our accurate model for blood pressure is not currently available to the public
  "pressure": {
    "phasic":{"0":{ "value": 0.5}}, 
  },
  "face": {
    // face states are keyed by specific time points (in seconds) and are detected for each frame
    "blinking":{"0":{ "value": false}},
    "talking":{"0":{ "value": false}},
  }
}
```
### Metrics struct

A `presage::smartspectra::formats::Metrics` struct instance `metrics` has the following layout:

| Field                                       | Description                                                              |
|---------------------------------------------|--------------------------------------------------------------------------|
| `metrics.pulse`                             | Contains everything related to pulse                                     |
| `metrics.pulse.values`                      | Pulse rate measurements with times and confidences (ordered)             |
| `metrics.pulse.trace`                       | Pulse waveform, or pleth (ordered points)                                |
| `metrics.pulse.strict`                      | The strict pulse rate (high confidence average over spot duration)       |
| `metrics.pulse.snr_sufficient`              | Whether signal-to-noise ratio was sufficient to compute strict pulse     |
| `metrics.breathing`                         | Contains everything related to breathing                                 |
| `metrics.breathing.values`                  | Breathing rate measurements with times and confidences (ordered)         |
| `metrics.breathing.upper_trace`             | Breathing movement waveform from chest (ordered points)                  |
| `metrics.breathing.upper_trace`             | Breathing movement waveform from abdomen (ordered points)                |
| `metrics.breathing.strict`                  | The strict breathing rate (high confidence average over spot duration)   |
| `metrics.breathing.snr_sufficient`          | Whether signal-to-noise ratio was sufficient to compute strict breathing |
| `metrics.breathing.amplitude`               | The amplitudes of breathing motion (ordered, with times)                 |
| `metrics.breathing.apnea`                   | Apnea detection results (ordered, with times)                            |
| `metrics.breathing.respiratory_line_length` | Respiratory line lengths (ordered, with times)                           |
| `metrics.breathing.inhale_exhale_ratio`     | Inhale-exhale ratios (ordered, with times)                               |
| `metrics.blooc_pressure`                    | Contains everything related to blood pressure                            |
| `metrics.blood_pressure.phasic`             | Phasic blood pressure measurements with times and confidences (ordered)  |
| `metrics.upload timestamp`                  | Date & time when the inputs were uploaded                                |
| `metrics.version`                           | Version of the Physiology REST API used                                  |
