###############
pixd for Python
###############

A Python port of pixd_, a tool for visualizing binary data using a colour palette. It is
in a lot of ways akin to a hexdump tool, except using coloured squares to
represent each octet.

.. image:: https://github.com/moreati/python-pixd/raw/master/meta/examples.png
   :target: https://github.com/moreati/python-pixd/blob/master/meta/examples.png
   :width: 664
   :alt: Screenshot

*pixd* uses 24-bit color SGR escape sequences.  For a list of terminal
emulators with support for these, see
`XVilka's list of supporting terminal emulators`_.

Usage
-----

To install

.. code:: console

    $ pip install pixd

To display a file

.. code:: console

    $ python -m pixd path/to/file


License
-------

MIT license.  Enjoy!

See also
--------

pixd_
  The original C implementation by FireyFly_

hexd_
  more conventional hexdump tool with colourful output, and also what *pixd*'s
  code is based on.

.. _FireyFly: http://github.com/FireyFly
.. _hexd: http://github.com/FireyFly/hexd
.. _pixd: http://github.com/FireyFly/pixd
.. _XVilka's list of supporting terminal emulators: https://gist.github.com/XVilka/8346728
