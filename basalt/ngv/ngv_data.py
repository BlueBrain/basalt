from builtins import map, range
import logging
import os

import h5py
import numpy as np


L = logging.getLogger(__name__)


class H5ContextManager(object):
    def __init__(self, filepath):
        self._fd = h5py.File(filepath, 'r')

    def close(self):
        self._fd.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


class CellData(H5ContextManager):
    def __init__(self, filepath):
        super(CellData, self).__init__(filepath)

        self.astrocyte_positions = self._fd['/positions']
        self.astrocyte_radii = self._fd['/radii']

        self.astrocyte_gids = self._fd['/ids']
        self.astrocyte_names = self._fd['/names']

    @property
    def astrocyte_point_data(self):
        return np.column_stack((self.astrocyte_positions, self.astrocyte_radii))

    @property
    def n_cells(self):
        return len(self.astrocyte_positions)


class MicrodomainTesselation(H5ContextManager):
    def __init__(self, filepath):
        super(MicrodomainTesselation, self).__init__(filepath)

        self._offset_t = {
            "points": 0,
            "triangles": 1,
            "neighbors": 2,
            "all": None,
            "domain_data": slice(0, 1),
        }

        self._offsets = self._fd['/offsets']
        self._connectivity = self._fd['/connectivity']

        self._raw_points = self._fd['/Data/points']
        self._raw_triangles = self._fd['/Data/triangles']
        self._raw_neighbors = self._fd['/Data/neighbors']

    def __iter__(self):
        for i in range(self.n_microdomains):
            yield self.domain_object(i)

    @property
    def n_microdomains(self):
        return len(self._offsets) - 1

    def _offset_slice(self, astrocyte_index, offset_type):
        return (
            self._offsets[astrocyte_index, offset_type],
            self._offsets[astrocyte_index + 1, offset_type],
        )

    def domain_neighbors(self, astrocyte_index):
        beg, end = self._offset_slice(astrocyte_index, self._offset_t['neighbors'])
        return self._raw_neighbors[beg:end]

    def domain_points(self, astrocyte_index):
        beg, end = self._offset_slice(astrocyte_index, self._offset_t['points'])
        return self._raw_points[beg:end]

    def domain_triangles(self, astrocyte_index):
        beg, end = self._offset_slice(astrocyte_index, self._offset_t['triangles'])
        return self._raw_triangles[beg:end]

    def domain_object(self, astrocyte_index):
        from morphspatial import ConvexPolygon

        return ConvexPolygon(
            self.domain_points(astrocyte_index), self.domain_triangles(astrocyte_index)
        )

    def iter_points(self):
        return map(self.domain_points, range(self.n_microdomains))

    def iter_triangles(self):
        return map(self.domain_triangles, range(self.n_microdomains))

    def iter_neighbors(self):
        return map(self.domain_neighbors, range(self.n_microdomains))


class GliovascularData(H5ContextManager):
    def __init__(self, filepath):
        super(GliovascularData, self).__init__(filepath)

        self.endfoot_graph_coordinates = self._fd['/endfoot_graph_coordinates']

        self.endfoot_surface_coordinates = self._fd['/endfoot_surface_coordinates']


class NeuroglialData(H5ContextManager):
    pass


class SynapticData(H5ContextManager):
    @property
    def synapse_coordinates(self):
        return self._fd['/synapse_coordinates']

    @property
    def n_synapses(self):
        return len(self.synapse_coordinates)


class NGVData(object):
    class _SpatialIndex(object):
        def __init__(self, ngv_config):

            self._config = ngv_config

        @property
        def synapses(self):
            raise NotImplementedError

        @property
        def vasculature(self):
            raise NotImplementedError

        @property
        def neuronal_somata(self):
            raise NotImplementedError

        @property
        def astrocytic_somata(self):
            raise NotImplementedError

    class _CellDataInfo(CellData):
        def __init__(self, ngv_config):
            filepath = ngv_config.output_paths('cell_data')
            super(NGVData._CellDataInfo, self).__init__(filepath)
            self._config = ngv_config

        def morphology_path(self, astrocyte_index):
            cell_name = self.astrocyte_names[astrocyte_index]
            return os.path.join(
                self.config.morphology_directory, '{}.h5'.format(cell_name)
            )

        def morphology_object(self, astrocyte_index):
            import morphio

            return morphio.Morphology(self.morphology_path(astrocyte_index))

    class _MicrodomainTesselationInfo(MicrodomainTesselation):
        def __init__(self, ngv_config):
            filepath = ngv_config.output_paths('overlapping_microdomain_structure')
            super(NGVData._MicrodomainTesselationInfo, self).__init__(filepath)
            self._config = ngv_config

        def domain_mesh_path(self, astrocyte_index):
            return os.path.join(
                self._config.microdomains_directory, '{}.stl'.format(astrocyte_index)
            )

        def domain_mesh_object(self, astrocyte_index):
            import stl

            return stl.Mesh.from_file(self.domain_mesh_path(astrocyte_index))

    class _GliovascularDataInfo(GliovascularData):
        def __init__(self, ngv_config):
            filepath = ngv_config.output_paths('gliovascular_data')
            super(NGVData._GliovascularDataInfo, self).__init__(filepath)
            self._config = ngv_config

        def endfoot_mesh_path(self, endfoot_index):
            return os.path.join(self.enfeet_directory, '{}.stl'.format(endfoot_index))

        def endfoot_mesh_object(self, endfoot_index):
            import stl

            return stl.Mesh.from_file(self.endfoot_mesh_path(endfoot_index))

    class _SynapticDataInfo(SynapticData):
        def __init__(self, ngv_config):
            filepath = ngv_config.output_paths('synaptic_data')
            super(NGVData._SynapticDataInfo, self).__init__(filepath)
            self._config = ngv_config

    def __init__(self, ngv_config):

        self._config = ngv_config

        self.cell_data = self._CellDataInfo(ngv_config)
        self.synaptic = self._SynapticDataInfo(ngv_config)
        # self.neuroglial   = NeuroglialData(ngv_config.output_paths('NeuroglialData'))
        self.microdomain = self._MicrodomainTesselationInfo(ngv_config)
        self.gliovascular = self._GliovascularDataInfo(ngv_config)

        self.spatial_index = self._SpatialIndex(ngv_config)

    def __enter__(self):
        self.synaptic.__enter__()
        self.cell_data.__enter__()
        # self.neuroglial.__enter__()
        self.microdomain.__enter__()
        self.gliovascular.__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def close(self):
        self.synaptic.close()
        self.cell_data.close()
        # self.neuroglial.close()
        self.microdomain.close()
        self.gliovascular.close()
