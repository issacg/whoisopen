# whoisopen
## Node.js extension to get foreground process in Windows

### Summary

```javascript
var whoisopen = require('whoisopen');
whoisopen(function(err, exe) {
    if (err)
        console.error(err);
    console.log("Open window is: " + exe);
});
```

Run from within Node.js this will usually return "Open window is: cmd.exe" or
"Open window is: node.exe"

### License

Copyright 2015 Issac Goldstand

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
