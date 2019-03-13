import logging

import h5py
import numpy as np

L = logging.getLogger(__name__)


class H5ContextManager(object):
    def __init__(self, filepath):
        self._fd = h5py.File(filepath, "r")

    def close(self):
        self._fd.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


class SynapticConnectivity(H5ContextManager):
    class _SynapseEntry(object):
        def __init__(self, fd):
            self._afferent_neuron = fd["/Synapse/Afferent Neuron"]

        def to_afferent_neuron(self, synapse_index):
            return self._afferent_neuron[synapse_index]

        @property
        def to_afferent_neuron_map(self):
            return self._afferent_neuron

    class _AfferentNeuronEntry(object):
        def __init__(self, fd):
            self._offsets = fd["/Afferent Neuron/offsets"]

        def _offset_slice(self, neuron_index):
            return self._offsets[neuron_index], self._offsets[neuron_index + 1]

        def to_synapse(self, neuron_index):
            beg, end = self._offset_slice(neuron_index)
            return np.arange(beg, end, dtype=np.uintp)

    def __init__(self, filepath):
        super(SynapticConnectivity, self).__init__(filepath)

        self.synapse = self._SynapseEntry(self._fd)
        self.afferent_neuron = self._AfferentNeuronEntry(self._fd)

    @property
    def n_neurons(self):
        return len(self.afferent_neuron._offsets) - 1

    @property
    def n_synapses(self):
        return len(self.synapse._afferent_neuron)


class GliovascularConnectivity(H5ContextManager):
    class _AstrocyteEntry(object):
        def __init__(self, fd):

            self._target_t = {"endfoot": 0, "vasculature_segment": 1}

            self._offset_t = {"endfoot": 0}

            self._connectivity = fd["/Astrocyte/connectivity"]
            self._offsets = fd["/Astrocyte/offsets"]

        def _offset_slice(self, astrocyte_index, offset_type):
            # right now the array is 1d because there is only
            # one offset
            return self._offsets[astrocyte_index], self._offsets[astrocyte_index + 1]

        def to_endfoot(self, astrocyte_index):
            beg, end = self._offset_slice(astrocyte_index, self._offset_t["endfoot"])
            return self._connectivity[beg:end, self._target_t["endfoot"]]

        def to_vasculature_segment(self, astrocyte_index):
            beg, end = self._offset_slice(astrocyte_index, self._offset_t["endfoot"])
            return self._connectivity[beg:end, self._target_t["vasculature_segment"]]

    class _EndfootEntry(object):
        def __init__(self, fd):

            self._target_t = {"astrocyte": 0, "vasculature_segment": 1}

            self._connectivity = fd["/Endfoot/connectivity"]

        def to_astrocyte(self, endfoot_index):
            return self._connectivity[endfoot_index, self._target_t["astrocyte"]]

        @property
        def to_astrocyte_map(self):
            return self._connectivity[:, self._target_t["astrocyte"]]

        def to_vasculature_segment(self, endfoot_index):
            return self._connectivity[
                endfoot_index, self._target_t["vasculature_segment"]
            ]

    class _VasculatureSegmentEntry(object):
        def __init__(self, fd):

            self._target_t = {"endfoot": 0, "astrocyte": 1}

            vasculature_group = fd["/Vasculature Segment"]
            self._connectivity = vasculature_group["connectivity"]

            self._min_index = self._connectivity.attrs["min_index"]
            self._max_index = self._connectivity.attrs["max_index"]

        def _is_index_valid(self, segment_index):
            return self._min_index <= segment_index < self._max_index

        def to_endfoot(self, segment_index):
            return (
                self._connectivity[
                    segment_index - self._min_index, self._target_t["endfoot"]
                ]
                if self._is_index_valid(segment_index)
                else -1
            )

        def to_astrocyte(self, segment_index):
            return (
                self._connectivity[
                    segment_index - self._min_index, self._target_t["astrocyte"]
                ]
                if self._is_index_valid(segment_index)
                else -1
            )

    def __init__(self, filepath):
        super(GliovascularConnectivity, self).__init__(filepath)

        self.endfoot = self._EndfootEntry(self._fd)
        self.astrocyte = self._AstrocyteEntry(self._fd)
        self.vasculature_segment = self._VasculatureSegmentEntry(self._fd)

    @property
    def n_astrocytes(self):
        return len(self.astrocyte._offsets) - 1

    @property
    def n_endfeet(self):
        return len(self.endfoot._connectivity)

    @property
    def edges_astrocyte_endfeet(self):
        e2a = self.endfoot.to_astrocyte_map
        endfeet_indices = np.arange(len(e2a), dtype=np.uintp)
        return np.column_stack((e2a, endfeet_indices))


class NeuroglialConnectivity(H5ContextManager):
    class _NeuronEntry(object):
        def __init__(self, fd):

            self._offsets = fd["/Neuron/offsets"]
            self._astrocyte = fd["/Neuron/astrocyte"]

        def _offset_slice(self, neuron_index):
            return self._offsets[neuron_index], self._offsets[neuron_index + 1]

        def to_astrocyte(self, neuron_index):
            beg, end = self._offset_slice(neuron_index)
            return self._astrocyte[beg:end]

    """
    class _SynapseEntry(object):

        def __init__(self, fd):

            self._offset_t = \
            {
                'astrocyte': 0
            }

            self._offsets = fd['/Synapse/offsets']
            self._astrocyte = fd['/Synapse/astrocyte']

        def _offset_slice(self, synapse_index, offset_type):
             return self._offsets[neuron_index, offset_type], \
                    self._offsets[neuron_index + 1, offset_type]

        def to_astrocyte(self, synapse_index):
            return self._connectivity[synapse_index, self._target_t['astrocyte']]
    """

    class _AstrocyteEntry(object):
        def __init__(self, fd):

            self._offset_t = {"synapse": 0, "neuron": 1}

            self._offsets = fd["/Astrocyte/offsets"]
            self._synapse = fd["/Astrocyte/synapse"]
            self._neuron = fd["/Astrocyte/neuron"]

        def _offset_slice(self, astrocyte_index, offset_type):
            return (
                self._offsets[astrocyte_index, offset_type],
                self._offsets[astrocyte_index + 1, offset_type],
            )

        def to_synapse(self, astrocyte_index):
            beg, end = self._offset_slice(astrocyte_index, self._offset_t["synapse"])
            return self._synapse[beg:end]

        def to_neuron(self, astrocyte_index):
            beg, end = self._offset_slice(astrocyte_index, self._offset_t["neuron"])
            return self._neuron[beg:end]

    def __init__(self, filepath):
        super(NeuroglialConnectivity, self).__init__(filepath)

        self.neuron = self._NeuronEntry(self._fd)
        # self.synapse = self._SynapseEntry(self._fd)
        self.astrocyte = self._AstrocyteEntry(self._fd)

    @property
    def n_astrocytes(self):
        return len(self.astrocyte._offsets) - 1

    @property
    def n_neurons(self):
        return len(self.neuron._offsets) - 1


class NGVConnectome(object):
    def __init__(self, ngv_config):

        self._snc = SynapticConnectivity(
            ngv_config.output_paths("synaptic_connectivity")
        )
        self._ngc = NeuroglialConnectivity(
            ngv_config.output_paths("neuroglial_connectivity")
        )
        self._gvc = GliovascularConnectivity(
            ngv_config.output_paths("gliovascular_connectivity")
        )

    def close(self):
        self._snc.close()
        self._ngc.close()
        self._gvc.close()

    # Astrocyte Stuff

    def astrocyte_endfeet(self, astrocyte_index):
        return self._gvc.astrocyte.to_endfoot(astrocyte_index)

    def astrocyte_vasculature_segments(self, astrocyte_index):
        return self._gvc.astrocyte.to_vasculature_segment(astrocyte_index)

    def astrocyte_synapses(self, astrocyte_index):
        return self._ngc.astrocyte.to_synapse(astrocyte_index)

    # Endfoot Stuff

    def endfoot_vasculature_segment(self, endfoot_index):
        return self._gvc.endfoot.to_vasculature_segment(endfoot_index)

    def endfoot_astrocyte(self, endfoot_index):
        return self._gvc.endfoot.to_astrocyte(endfoot_index)

    # Synapse Stuff

    def synapse_afferent_neuron(self, synapse_index):
        return self._snc.synapse.to_afferent_neuron(synapse_index)

    def synapse_astrocyte(self, synapse_index):
        return self._ngc.synapse.to_astrocyte(synapse_index)

    # Vasculature Segment Stuff

    def vasculature_segment_endfoot(self, vasculature_segment_index):
        return self._gvc.vasculature_segment.to_endfoot(vasculature_segment_index)

    def vasculature_segment_astrocyte(self, vasculature_segment_index):
        return self._gvc.vasculature_segment.to_astrocyte(vasculature_segment_index)

    # Pairwise Combinations

    def astrocyte_afferent_neurons(self, astrocyte_index):
        syn_idx = self.astrocyte_synapses(astrocyte_index)
        return set(self.synapse_afferent_neuron(index) for index in syn_idx)
