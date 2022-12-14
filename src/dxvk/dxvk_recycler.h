#pragma once

#include <mutex>
#include <vector>

namespace dxvk {
  
  /**
   * \brief Object recycler
   * 
   * Implements a thread-safe buffer that can store up to
   * a given number of objects of a certain type. This way,
   * DXVK can efficiently reuse and reset objects instead
   * of destroying them and creating them anew.
   * \tparam T Type of the objects to store
   * \tparam N Maximum number of objects to store
   */
  template<typename T, size_t N>
  class DxvkRecycler {
    
  public:
    
    /**
     * \brief Retrieves an object if possible
     * 
     * Returns an object that was returned to the recycler
     * earier. In case no objects are available, this will
     * return \c nullptr and a new object has to be created.
     * \return An object, or \c nullptr
     */
    Rc<T> retrieveObject() {
      std::lock_guard<dxvk::mutex> lock(m_mutex);
      
      if (m_get == m_put)
        return nullptr;
      
      return std::exchange(m_objects[(m_get++) % N], Rc<T>());
    }
    
    /**
     * \brief Returns an object to the recycler
     * 
     * If the buffer is full, the object will be destroyed
     * once the last reference runs out of scope. No further
     * action needs to be taken in this case.
     * \param [in] object The object to return
     */
    void returnObject(const Rc<T>& object) {
      std::lock_guard<dxvk::mutex> lock(m_mutex);
      
      if (m_put - m_get < N)
        m_objects[(m_put++) % N] = object;
    }
    
  private:
    
    dxvk::mutex           m_mutex;
    std::array<Rc<T>, N>  m_objects;

    uint64_t              m_get = 0;
    uint64_t              m_put = 0;
    
  };
  
}