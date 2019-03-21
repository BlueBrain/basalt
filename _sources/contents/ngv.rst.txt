NGV
###

Graph class
===========

Basalt Python package comes with a definition of a graph to help the placement
of astrocytes. See :class:`basalt.ngv.NGVGraph`

It implements the following graph typology:

.. image:: /_static/ngv-graph.png

Payloads
========

Efficient payloads classes have been implemented in C++ and exposed in Python bindings:

* :class:`basalt.ngv.Astrocyte`
* :class:`basalt.ngv.Neuron`
* :class:`basalt.ngv.EdgeAstrocyteSegment`
* :class:`basalt.ngv.Segment`
* :class:`basalt.ngv.Synapse`
* :class:`basalt.ngv.MicroDomain`
* :class:`basalt.ngv.Point`

