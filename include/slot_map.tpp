// included by slot_map.h to implement template methods

namespace knight {

template<typename T>
ID SlotMap<T>::Create() {
  // Are there no spare entities?
  if (free_list_.empty()) {
    // Create free entry for new entity in chunk
    for (int i = kChunkSize - 1; i >= 0; --i) {
      free_list_.push_back(slot_table_.size() * kChunkSize + i);
    }

    // Add new chunk to table
    slot_table_.push_back(Chunk(new T[kChunkSize]));
  }

  uint32_t freeId = free_list_.back();
  free_list_.pop_back();

  // get first free object
  T *obj = &slot_table_[freeId / kChunkSize][freeId % kChunkSize];

  // Update index
  ID id = obj->id();
  id.index = freeId;
  obj->set_id(id);

  // TODO: reset object

  return id;
}

template<typename T>
T *SlotMap<T>::Get(const ID &id) const {
  uint32_t chunkIndex = id.index / kChunkSize;

  // Does the chunk exist?
  if (chunkIndex < slot_table_.size()) {
    T *obj = &slot_table_[chunkIndex][id.index % kChunkSize];

    // If the ids (specifically the versions) match return object else NULL
    return obj->id() != id ? nullptr : obj;
  } else {

    // Chunk doesn't exist return NULL
    return nullptr;
  }
}

template<typename T>
void SlotMap<T>::Destroy(ID id) {
  T *obj = Get(id);

  if (obj != nullptr) {
    // Increment version to generate unique id
    id.version++;
    obj->set_id(id);
    free_list_.push_back(id.index);
  } else {
    ERR("Trying to delete non-existent object: %lu", id.id);
  }
}

} // namespace knight