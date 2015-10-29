minitail
===

`tail`-like library for C++.

Usage
---

The `minitail` class is initialized with a file to watch and a delegate function which will be called every time a new line is read with its contents as the parameter.

Use start() and stop() to control the object.

The object runs **asynchronously**.

License
---

Published under a BSD 3-Clause License.
