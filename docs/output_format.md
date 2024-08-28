### Data Format

The metrics JSON object has the following format (annotated using JSON5 comments) :

```json5
{
  "error": "", // error string, if any
  "version": "3.10.1", // Physiology version
  "pulse": { // pulse metrics
    "hr":{ // heart/pulse rate
      "10":{ // seconds
        "value": 58.9, 
        "confidence": 0.95 // confidence score based on noise-to-signal ratio
      },
      "11":{
        "value": 58.2,
        "confidence": 0.94
      },
      "12":{
        "value": 58.1,
        "confidence": 0.91
      }
    },
    "hr_trace":{
      "0":{ "value": 0.5},
      "0.033":{ "value": 0.56},
      "0.066":{ "value": 0.59}
    },
    "hr_spec":{
      "10":{ "value": [], "freq":[]},
      "11":{ "value": [], "freq":[]},
      "12":{ "value": [], "freq":[]}
    },
    "hrv":{}
  },
  "breath": { // breath metrics
    "rr":{
      "15":{
        "value": 18.9,
        "confidence": 0.95
      },
      "16":{
        "value": 18.2,
        "confidence": 0.94
      },
      "17":{
        "value": 18.1,
        "confidence": 0.91
      }
    },
    "rr_trace":{
      "0":{ "value": 0.5},
      "0.033":{ "value": 0.56},
      "0.066":{ "value": 0.59}
    },
    "rr_trace_lower":{
      "0":{ "value": 0.5},
      "0.033":{ "value": 0.56},
      "0.066":{ "value": 0.59}
    },
    "rr_spec":{
      "15":{ "value": [], "freq":[]},
      "16":{ "value": [], "freq":[]},
      "17":{ "value": [], "freq":[]}
    },
    "rrl":{"0":{ "value": 0.5}},
    "apnea":{"0":{ "value": false}}},
    "ie":{"0":{ "value": 1.5}},
    "amplitude":{"0":{ "value": 0.5}},
    "baseline":{"0":{ "value": 0.5}}
  },
  "pressure": {
    "phasic":{"0":{ "value": 0.5}},
  },
  "face": {
    "landmarks":{"0":{ "value": [[]]]}},
    "blinking":{"0":{ "value": false}},
    "talking":{"0":{ "value": false}},
  }
}
```
