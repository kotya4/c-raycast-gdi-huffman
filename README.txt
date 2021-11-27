RAY, stands for RAY AN YUM ( because yums are yummy )

disadvantages of ./build/huskdump~211127.zip:
  text buffer renders incorrectly

disadvantages of ./build/raybuild~210703.zip:
  renderer bug on some camera angles in usual version, cured
  not showing music info on screen, pending
  executable size is too big ( maybe because of images? ), pending
  for some reason gdi works faster than sdl when floor is casting, but also slower than sdl on fullscreen emulation.

build silently and run re ( raycast engine )
  `make`

build re for debugging
  `make d`

build re for release (\w optimisations)
  `make r`

build and run test
  `make t`

idk some transpilation stuff going on here, generates compressed images for re
  `make i`

builds husk, husk is bash-like command interpreter
  `make husk`

builds husk in release mode
  `make huskr`
