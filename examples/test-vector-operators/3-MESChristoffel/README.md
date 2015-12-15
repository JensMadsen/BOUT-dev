# 3-MESChristoffel

Contains a BOUT++ implementation which uses the Method of Exact Solution
to verify the Christoffel symbols

Folders present are:

* [calculations](/calculations/) contains a ipython notebook (see
  [jupyter](http://jupyter.org/) for more details (can by the way be viewed on
  the github page)) of derivation of the coordinate system used in this folder
* [data](/data/) contains the standard BOUT.inp file.
* [includeCov](/includeCov/) contains same as in [data](/data/), but the
  covariant metric tensors are also explicitly written (this is good if the
  BOUT++ inversion routine has trouble inverting the contravariant metric
  tensor)
* [pythonRoutines](/pythonRoutines/) contains post-processing routines

Driver files present are:

* [driverCheckChristoffel.py](driverCheckChristoffel.py) bout_runners driver
  which plots the Christoffel symbols
* [driverChristoffelConvergence.py](driverChristoffelConvergence.py) bout_runners driver
  which makes the convergence test
* [driverChristoffelConvergenceIncludeCov.py](driverChristoffelConvergenceIncludeCov.py)
  bout_runners driver which makes the convergence test where also the covariant
  metric tensors are written in the input file