import http.server
import functools
import os.path as osp

from ._basalt import Status, Vertices, Edges, Graph, make_id
from ._basalt import __version__, __rocksdb_version__  # noqa


__all__ = ["Graph", "Status", "make_id", "Edges", "Vertices", "serve_doc"]


def serve_doc(bind="", port=8000):
    """Spawn a HTTP server serving basalt documentation and block

    Args:
        bind(str): network interface to use (default all)
        port(int): network port to open
    """
    doc_dir = osp.join(osp.dirname(__file__), "docs", "html")
    handler_class = functools.partial(
        http.server.SimpleHTTPRequestHandler, directory=doc_dir
    )
    http.server.test(HandlerClass=handler_class, port=port, bind=bind)
