
#include <stdio.h>
#include <stdlib.h>
#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <python3.7/Python.h>


// {{{ struct my_error_mgr
struct my_error_mgr 
{
   struct jpeg_error_mgr pub;	
   jmp_buf setjmp_buffer;	
};
typedef struct my_error_mgr * my_error_ptr;
// }}}

// {{{ my_output_message()
METHODDEF(void) my_output_message (j_common_ptr cinfo)
{
   char buffer[JMSG_LENGTH_MAX];
   (*cinfo->err->format_message) (cinfo, buffer);
   fprintf(stderr, "Error: %s\n", buffer);
}
// }}}

// {{{ my_error_exit()
METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
   char buffer[JMSG_LENGTH_MAX];
   my_error_ptr myerr = (my_error_ptr) cinfo->err;
   (*cinfo->err->format_message) (cinfo, buffer);
   fprintf(stderr, "Error: %s\n", buffer);
   longjmp(myerr->setjmp_buffer, 1);
}
// }}}

// {{{ dict_add_int()
PyObject* dict_add_int(PyObject* dict, const char *key, int value)
{
   PyObject *py_key = Py_BuildValue("s", key);
   PyObject *py_value = Py_BuildValue("i", value);
   PyDict_SetItem(dict, py_key, py_value);
   Py_DecRef(py_key);
   Py_DecRef(py_value);

   return dict;
}
// }}}

// {{{ dict_add_float()
PyObject* dict_add_float(PyObject* dict, const char *key, float value)
{
   PyObject *py_key = Py_BuildValue("s", key);
   PyObject *py_value = Py_BuildValue("f", value);
   PyDict_SetItem(dict, py_key, py_value);
   Py_DecRef(py_key);
   Py_DecRef(py_value);

   return dict;
}
// }}}

// {{{ list_append_float()
PyObject* list_append_float(PyObject* list, float value)
{
   PyObject *py_value = Py_BuildValue("f", value);
   PyList_Append(list, py_value);
   Py_DecRef(py_value);

   return list;
}
// }}}

// {{{ read_file()
PyObject* read_file(const char *path)
{
   PyObject *result = PyDict_New();
   assert(PyDict_Check(result));

   struct jpeg_decompress_struct cinfo;
   struct my_error_mgr jerr;
   FILE *f = NULL;
   PyObject *py_key = NULL;
   PyObject *py_value = NULL;
   PyObject *row = NULL;
   PyObject *col = NULL;
   PyObject *dict = NULL;
   PyObject *list = NULL;

   if((f = fopen(path, "rb")) == NULL)
   {
      fprintf(stderr, "Can not open file: %s\n", path);
      return result;
   }

   /* set up the normal JPEG error routines, then override error_exit. */
   cinfo.err = jpeg_std_error(&jerr.pub);
   jerr.pub.error_exit = my_error_exit;
   jerr.pub.output_message = my_output_message;

   /* establish the setjmp return context for my_error_exit to use. */
   if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(f);
      fprintf(stderr, "Error reading file: %s\n", path);
      return result;
   }

   /* initialize JPEG decompression object */
   jpeg_create_decompress(&cinfo);
   jpeg_stdio_src(&cinfo, f);

   /* save contents of markers */
   jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);

   /* read header and coefficients */
   jpeg_read_header(&cinfo, TRUE);

  /* Set out_color_components */
   switch (cinfo.out_color_space) {
      case JCS_GRAYSCALE:
         cinfo.out_color_components = 1;
         break;
      case JCS_RGB:
         cinfo.out_color_components = 3;
         break;
      case JCS_YCbCr:
         cinfo.out_color_components = 3;
         break;
      case JCS_CMYK:
         cinfo.out_color_components = 4;
         break;
      case JCS_YCCK:
         cinfo.out_color_components = 4;
         break;
      default:
         fprintf(stderr, "Unknown color space: %d\n", cinfo.out_color_space);
         return result;
   }

   // {{{ Header info 
   result = dict_add_int(result, "image_width", cinfo.image_width);
   result = dict_add_int(result, "image_height", cinfo.image_height);
   result = dict_add_int(result, "image_color_space", cinfo.out_color_space);
   result = dict_add_int(result, "image_components", cinfo.out_color_components);
   result = dict_add_int(result, "jpeg_color_space", cinfo.jpeg_color_space);
   result = dict_add_int(result, "jpeg_components", cinfo.num_components);
   result = dict_add_int(result, "progressive_mode", cinfo.progressive_mode);
   // }}}

   // {{{ Components info
   PyObject* comp_info = PyList_New(0);
   assert(PyList_Check(comp_info));

   for(size_t ci = 0; ci < cinfo.num_components; ci++) 
   {
      PyObject *comp = PyDict_New();
      assert(PyDict_Check(comp));

      comp = dict_add_int(comp, "component_id", cinfo.comp_info[ci].component_id);
      comp = dict_add_int(comp, "h_samp_factor", cinfo.comp_info[ci].h_samp_factor);
      comp = dict_add_int(comp, "v_samp_factor", cinfo.comp_info[ci].v_samp_factor);
      comp = dict_add_int(comp, "quant_tbl_no", cinfo.comp_info[ci].quant_tbl_no);
      comp = dict_add_int(comp, "ac_tbl_no", cinfo.comp_info[ci].ac_tbl_no);
      comp = dict_add_int(comp, "dc_tbl_no", cinfo.comp_info[ci].dc_tbl_no);

      PyList_Append(comp_info, comp);
      Py_DecRef(comp);
   }

   py_key = Py_BuildValue("s", "comp_info");
   PyDict_SetItem(result, py_key, comp_info);
   Py_DecRef(py_key);

   Py_DecRef(comp_info);
   // }}}

   // {{{ Quantization tables 
   PyObject* quant_tables = PyList_New(0);
   assert(PyList_Check(quant_tables));

   for(size_t n = 0; n < NUM_QUANT_TBLS; n++) 
   {
      if(cinfo.quant_tbl_ptrs[n] != NULL) 
      {
         JQUANT_TBL *quant_ptr = cinfo.quant_tbl_ptrs[n];
         
         row = PyList_New(0);
         assert(PyList_Check(row));
         for(size_t i = 0; i < DCTSIZE; i++) 
         {
            col = PyList_New(0);
            assert(PyList_Check(col));
            for(size_t j = 0; j < DCTSIZE; j++)
            {
               py_value = Py_BuildValue("f", (double) quant_ptr->quantval[i*DCTSIZE+j]);
               PyList_Append(col, py_value);
               Py_DecRef(py_value);
            }
            PyList_Append(row, col);
            Py_DecRef(col);
         }
         PyList_Append(quant_tables, row);
         Py_DecRef(row);
         
      }
   }

   py_key = Py_BuildValue("s", "quant_tables");
   PyDict_SetItem(result, py_key, quant_tables);
   Py_DecRef(py_key);
   // }}}

   // {{{ AC Huffman tables 
   PyObject* ac_huff_tables = PyList_New(0);
   assert(PyList_Check(ac_huff_tables));

   for(size_t n = 0; n < NUM_HUFF_TBLS; n++) 
   {
      if(cinfo.ac_huff_tbl_ptrs[n] != NULL) 
      {
         JHUFF_TBL *huff_ptr = cinfo.ac_huff_tbl_ptrs[n];

         dict = PyDict_New();
         assert(PyDict_Check(dict));

         row = PyList_New(0);
         assert(PyList_Check(row));
         for(size_t i=1; i<=16; i++)
            list_append_float(row, huff_ptr->bits[i]);
         
         py_key = Py_BuildValue("s", "counts");
         PyDict_SetItem(dict, py_key, row);
         Py_DecRef(py_key);
         Py_DecRef(row);

         row = PyList_New(0);
         assert(PyList_Check(row));
         for(size_t i=0; i<256; i++)
            list_append_float(row, huff_ptr->huffval[i]);
         
         py_key = Py_BuildValue("s", "symbols");
         PyDict_SetItem(dict, py_key, row);
         Py_DecRef(py_key);
         Py_DecRef(row);

         PyList_Append(ac_huff_tables, dict);
         Py_DecRef(dict);
      }
   }

   py_key = Py_BuildValue("s", "ac_huff_tables");
   PyDict_SetItem(result, py_key, ac_huff_tables);
   Py_DecRef(py_key);
   // }}}

   // {{{ DC Huffman tables 
   PyObject* dc_huff_tables = PyList_New(0);
   assert(PyList_Check(dc_huff_tables));

   for(size_t n = 0; n < NUM_HUFF_TBLS; n++) 
   {
      if(cinfo.dc_huff_tbl_ptrs[n] != NULL) 
      {
         JHUFF_TBL *huff_ptr = cinfo.dc_huff_tbl_ptrs[n];

         dict = PyDict_New();
         assert(PyDict_Check(dict));

         row = PyList_New(0);
         assert(PyList_Check(row));
         for(size_t i=1; i<=16; i++)
            list_append_float(row, huff_ptr->bits[i]);
         
         py_key = Py_BuildValue("s", "counts");
         PyDict_SetItem(dict, py_key, row);
         Py_DecRef(py_key);
         Py_DecRef(row);

         row = PyList_New(0);
         assert(PyList_Check(row));
         for(size_t i=0; i<256; i++)
            list_append_float(row, huff_ptr->huffval[i]);
         
         py_key = Py_BuildValue("s", "symbols");
         PyDict_SetItem(dict, py_key, row);
         Py_DecRef(py_key);
         Py_DecRef(row);

         PyList_Append(dc_huff_tables, dict);
         Py_DecRef(dict);
      }
   }

   py_key = Py_BuildValue("s", "dc_huff_tables");
   PyDict_SetItem(result, py_key, dc_huff_tables);
   Py_DecRef(py_key);
   // }}}

   // {{{ DCT coefficients
   jvirt_barray_ptr *coefs = jpeg_read_coefficients(&cinfo);

   PyObject* coef_arrays = PyList_New(0);
   assert(PyList_Check(coef_arrays));



   for(size_t ci=0; ci<cinfo.num_components; ci++) 
   {
      jpeg_component_info *compptr = cinfo.comp_info + ci;

      list = PyList_New(0);
      assert(PyList_Check(list));
      
      for(size_t blk_y=0; blk_y<compptr->height_in_blocks; blk_y++)
      {
         JBLOCKARRAY buffer = (cinfo.mem->access_virt_barray)
    	   ((j_common_ptr) &cinfo, coefs[ci], blk_y, 1, FALSE);
         for(size_t blk_x=0; blk_x<compptr->width_in_blocks; blk_x++)
         {
            row = PyList_New(0);
            assert(PyList_Check(row));
            JCOEFPTR bufptr = buffer[0][blk_x];
            for(size_t i=0; i<DCTSIZE; i++)
            {
               col = PyList_New(0);
               assert(PyList_Check(col));
               for (size_t j=0; j<DCTSIZE; j++)
               {
                  py_value = Py_BuildValue("f", (double) bufptr[i*DCTSIZE+j]);
                  PyList_Append(col, py_value);
                  Py_DecRef(py_value);
               }
               PyList_Append(row, col);
               Py_DecRef(col);
            }
            PyList_Append(list, row);
            Py_DecRef(row);
         }
      }

      PyList_Append(coef_arrays, list);
      Py_DecRef(list);
   }

   py_key = Py_BuildValue("s", "coef_arrays");
   PyDict_SetItem(result, py_key, coef_arrays);
   Py_DecRef(py_key);
   // }}}

   jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);
   fclose(f);

   return result;
}
// }}}





