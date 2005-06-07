/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
/**
 * @file
 */
#ifndef MCA_PML_BASE_SEND_REQUEST_H
#define MCA_PML_BASE_SEND_REQUEST_H

#include "ompi_config.h"
#include "mca/pml/pml.h"
#include "datatype/datatype.h"
#include "mca/pml/base/pml_base_request.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
OMPI_DECLSPEC extern ompi_class_t mca_pml_base_send_request_t_class;


/**
 * Base type for send requests 
 */
struct mca_pml_base_send_request_t {
    mca_pml_base_request_t req_base;         /** base request type - common data structure for use by wait/test */
    ompi_datatype_t* req_datatype;           /**< pointer to datatype */
    void *req_addr;                          /**< pointer to send buffer - may not be application buffer */
    size_t req_count;                        /**< number of elements in send buffer */
    size_t req_bytes_packed;                 /**< packed size of a message given the datatype and count */
    mca_pml_base_send_mode_t req_send_mode;  /**< type of send */
    ompi_convertor_t req_convertor;          /**< convertor that describes this datatype */
};
typedef struct mca_pml_base_send_request_t mca_pml_base_send_request_t;



/**
 * Initialize a send request with call parameters.
 *
 * @param request (IN)     Send request
 * @param addr (IN)        User buffer
 * @param count (IN)       Number of elements of indicated datatype.
 * @param datatype (IN)    User defined datatype
 * @param peer (IN)        Destination rank
 * @param tag (IN)         User defined tag
 * @param comm (IN)        Communicator
 * @param mode (IN)        Send mode (STANDARD,BUFFERED,SYNCHRONOUS,READY)
 * @param persistent (IN)  Is request persistent.
 *
 * Perform a any one-time initialization. Note that per-use initialization
 * is done in the send request start routine.
 */

#define MCA_PML_BASE_SEND_REQUEST_INIT( request,                          \
                                        addr,                             \
                                        count,                            \
                                        datatype,                         \
                                        peer,                             \
                                        tag,                              \
                                        comm,                             \
                                        mode,                             \
                                        persistent)                       \
   {                                                                      \
      /* increment reference count on communicator */                     \
      OBJ_RETAIN(comm);                                                   \
      OBJ_RETAIN(datatype);                                               \
                                                                          \
      OMPI_REQUEST_INIT(&(request)->req_base.req_ompi);                   \
      request->req_addr = addr;                                           \
      request->req_count = count;                                         \
      request->req_datatype = datatype;                                   \
      request->req_send_mode = mode;                                      \
      request->req_base.req_addr = addr;                                  \
      request->req_base.req_count = count;                                \
      request->req_base.req_datatype = datatype;                          \
      request->req_base.req_peer = (int32_t)peer;                         \
      request->req_base.req_tag = (int32_t)tag;                           \
      request->req_base.req_comm = comm;                                  \
      request->req_base.req_proc = ompi_comm_peer_lookup(comm,peer);      \
      request->req_base.req_persistent = persistent;                      \
      request->req_base.req_pml_complete = (persistent ? true : false);   \
      request->req_base.req_free_called = false;                          \
      request->req_base.req_ompi.req_status._cancelled = 0;               \
                                                                          \
      /* initialize datatype convertor for this request */                \
      if(count > 0) {                                                     \
         ompi_convertor_copy( request->req_base.req_proc->proc_convertor, \
                              &request->req_convertor);                   \
         /* We will create a convertor specialized for send       */      \
         /* just to be able to get the packed size. This size     */      \
         /* depend on the conversion used sender side or receiver */      \
         /* size. BEWARE this convertor is not suitable for the   */      \
         /* sending operation !!                                  */      \
         ompi_convertor_init_for_send( &request->req_convertor,           \
                                       0,                                 \
                                       request->req_base.req_datatype,    \
                                       request->req_base.req_count,       \
                                       request->req_base.req_addr,        \
                                       -1, NULL );                        \
         ompi_convertor_get_packed_size( &request->req_convertor,         \
                              (uint32_t*)&(request->req_bytes_packed) );  \
      } else {                                                            \
         request->req_bytes_packed = 0;                                   \
      }                                                                   \
   }

/**
 *  Release the ref counts on the communicator and datatype.
 *
 *  @param request (IN)    The send request.
 */

#define MCA_PML_BASE_SEND_REQUEST_RETURN( request )                       \
    do {                                                                  \
        OBJ_RELEASE((request)->req_base.req_comm);                        \
        OBJ_RELEASE((request)->req_base.req_datatype);                    \
    } while (0)


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif
#endif

