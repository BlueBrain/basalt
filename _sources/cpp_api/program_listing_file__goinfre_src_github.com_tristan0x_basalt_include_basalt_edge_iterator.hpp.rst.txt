
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_edge_iterator.hpp:

Program Listing for File edge_iterator.hpp
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_edge_iterator.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/edge_iterator.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <iterator>
   #include <memory>
   
   #include <basalt/fwd.hpp>
   
   namespace basalt {
   
   class EdgeIterator: public std::iterator<std::input_iterator_tag, const edge_uid_t> {
     public:
       template <EdgeOrientation Orientation>
       EdgeIterator(const GraphImpl<Orientation>& pimpl, size_t from);
   
       EdgeIterator(const EdgeIterator& other);
   
       EdgeIterator& operator++();
   
       const EdgeIterator operator++(int);
   
       bool operator==(const EdgeIterator& other) const;
   
       bool operator!=(const EdgeIterator& other) const;
   
       const value_type& operator*();
   
       using EdgeIteratorImpl_ptr = std::shared_ptr<EdgeIteratorImpl>;
   
     private:
       EdgeIteratorImpl_ptr pimpl_;
   };
   
   extern template EdgeIterator::EdgeIterator(
       const basalt::GraphImpl<EdgeOrientation::directed>& pimpl,
       size_t from);
   extern template EdgeIterator::EdgeIterator(
       const basalt::GraphImpl<EdgeOrientation::undirected>& pimpl,
       size_t from);
   
   }  // namespace basalt
