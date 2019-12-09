
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_status.hpp:

Program Listing for File status.hpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_status.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/status.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <iosfwd>
   #include <string>
   
   #include <basalt/fwd.hpp>
   
   namespace basalt {
   
   struct Status {
       enum Code {
           ok_code = 0,
           not_implemented_code = -1,
           missing_vertex_code = -2,
           invalid_edge_code = -3,
           missing_edge_code = -4,
       };
   
   
       Status(Code code, std::string message);
       Status() = delete;
       inline bool operator!() const noexcept {
           return code != 0;
       }
       inline explicit operator bool() const noexcept {
           return code == 0;
       }
   
       const Status& raise_on_error() const;
   
       static const Status& error_not_implemented();
   
       static Status error_missing_vertex(const vertex_uid_t& vertex);
   
       static Status error_missing_edge(const edge_uid_t& edge);
   
       static Status error_invalid_edge(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2);
   
       static const Status& ok();
   
       const Code code;
       const std::string message;
   };
   
   }  // namespace basalt
   
   std::ostream& operator<<(std::ostream& ostr, const basalt::Status& status);
