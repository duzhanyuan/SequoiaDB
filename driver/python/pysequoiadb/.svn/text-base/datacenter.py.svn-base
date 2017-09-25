#   Copyright (C) 2012-2014 SequoiaDB Ltd.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

try:
   import sdb
except ImportError:
   raise Exception("Cannot find extension: sdb")

import bson
import pysequoiadb
from pysequoiadb.common import const
from pysequoiadb.error import (SDBBaseError, SDBEndOfCursor)

class datacenter(object):
   """Data center of SequoiaDB
   """

   def __init__(self):
      self._dc = None
      try:
         self._dc = sdb.create_dc();
      except SystemError:
         raise SDBBaseError("Failed to alloc data center object", const.SDB_OOM)

   def __del__(self):
      if self._dc is not None:
         try:
            rc = sdb.release_dc(self._cursor)
            pysequoiadb._raise_if_error("Failed to release data center", rc)
         except SDBBaseError:
            raise
         self._dc = None

   def __repr__(self):
      return "Data Center: %s" % (self.get_name())

   def create_image(self, addr):
      """Create an image by specified address

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """

      if not isinstance(addr, basestring):
         raise TypeError("addr must be an instance of basestring")

      try:
         rc = sdb.dc_create_image(self._dc, addr)
         pysequoiadb._raise_if_error("Failed to create data center", rc)
      except SDBBaseError:
         raise

   def remove_image(self):
      """Remove an image

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_remove_image(self._dc)
         pysequoiadb._raise_if_error("Failed to remove data center", rc)
      except SDBBaseError:
         raise

   def attach_groups(self, info):
      """Add groups to data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      if not isinstance(info, dict):
         raise TypeError("info must be an instance of dict")
      try:
         rc = sdb.dc_attach_groups(self._dc, info)
         pysequoiadb._raise_if_error("Failed to attach groups to data center", rc)
      except SDBBaseError:
         raise

   def detach_groups(self, info):
      """Remove groups from data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      if not isinstance(info, dict):
         raise TypeError("info must be an instance of dict")
      try:
         rc = sdbg.dc_detach_groups(self._dc, info)
         pysequoiadb._raise_if_error("Failed to detach groups from data center", rc)
      except SDBBaseError:
         raise

   def enable_image(self):
      """Enable image in data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_enable_image(self._dc)
         pysequoiadb._raise_if_error("Failed to enable image", rc)
      except SDBBaseError:
         raise

   def disable_image(self):
      """Disable image in data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_disable_image(self._dc)
         pysequoiadb._raise_if_error("Failed to disable image", rc)
      except SDBBaseError:
         raise

   def activate(self):
      """Active data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_activate(self._dc);
         pysequoiadb._raise_if_error("Failed to make date center activate", rc)
      except SDBBaseError:
         raise

   def deactivate(self):
      """Deactive data center

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_deactivate(self._dc)
         pysequoiadb._raise_if_error("Failed to make data center deactive")
      except SDBBaseError:
         raise

   def enable_read_only(self):
      """make data center to be read-only

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_enable_read_only(self._dc)
         pysequoiadb._raise_if_error("Failed to make data center read-only", rc)
      except SDBBaseError:
         raise

   def disabel_read_only(self):
      """Disable data center to be read-only

      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc = sdb.dc_disable_read_only(self._dc)
         pysequoiadb._raise_if_error("Failed to disable data center read-only", rc)
      except SDBBaseError:
         raise

   def get_detail(self):
      """Get detail info of data center

      Return values:
         a dict object of detail info
      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc, detail = sdb.dc_get_detail(self._dc)
         if const.SDB_OK == rc:
            record, size = bson._bson_to_dict(detail, dict, False,
                                              bson.OLD_UUID_SUBTYPE, True)
         pysequoiadb._raise_if_error("Failed to get detail info of data center", rc)
      except SDBBaseError:
         raise

      return record

   def get_name(self):
      """Get data center name

      Return values:
         the name of data center
      Exceptions:
         pysequoiadb.error.SDBBaseError
      """
      try:
         rc, name = sdb.dc_get_name(self._dc)
         pysequoiadb._raise_if_error("Failed to get name of data center", rc)
      except SDBBaseError:
         raise

      return name
