/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2013 Facebook, Inc. (http://www.facebook.com)     |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef incl_HPHP_EXT_COLLECTION_H_
#define incl_HPHP_EXT_COLLECTION_H_

#include "hphp/runtime/ext/base_vector.h"
#include "hphp/runtime/base/base-includes.h"
#include "hphp/system/systemlib.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/**
 * Called by the JIT on an emitVectorSet().
 */
void triggerCow(c_Vector* vec);
ArrayIter getArrayIterHelper(CVarRef v, size_t& sz);
TypedValue* cvarToCell(const Variant* v);

///////////////////////////////////////////////////////////////////////////////
// class Vector

FORWARD_DECLARE_CLASS(Vector);
class c_Vector : public BaseVector {
 public:
  DECLARE_CLASS_NO_SWEEP(Vector)

 public:

  explicit c_Vector(Class* cls = c_Vector::classof());

  void t___construct(CVarRef iterable = null_variant);
  Object t_add(CVarRef val);
  Object t_addall(CVarRef val);
  Object t_append(CVarRef val); // deprecated
  Variant t_pop();
  void t_resize(CVarRef sz, CVarRef value);
  void t_reserve(CVarRef sz);
  Object t_clear();
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_keys();
  Object t_values();
  Object t_lazy();
  Object t_kvzip();
  Variant t_at(CVarRef key);
  Variant t_get(CVarRef key);
  Object t_set(CVarRef key, CVarRef value);
  Object t_setall(CVarRef iterable);
  Object t_put(CVarRef key, CVarRef value); // deprecated
  bool t_contains(CVarRef key); // deprecated
  bool t_containskey(CVarRef key);
  Object t_removekey(CVarRef key);
  Array t_toarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_tomap();
  Object t_tostablemap();
  Object t_toset();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  void t_reverse();
  void t_splice(CVarRef offset, CVarRef len = uninit_null(),
                CVarRef replacement = uninit_null());
  int64_t t_linearsearch(CVarRef search_value);
  void t_shuffle();
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_mapwithkey(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_filterwithkey(CVarRef callback);
  Object t_zip(CVarRef iterable);
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);
  static Object ti_fromitems(CVarRef iterable);
  static Object ti_fromarray(CVarRef arr); // deprecated
  static Object ti_slice(CVarRef vec, CVarRef offset,
                         CVarRef len = uninit_null());
  static Object t_slice(CVarRef vec, CVarRef offset,
                        CVarRef len = uninit_null()) {
    return ti_slice(vec, offset, len);
  }

  static void throwOOB(int64_t key) ATTRIBUTE_NORETURN;

  void set(int64_t key, TypedValue* val) {
    assert(val->m_type != KindOfRef);
    if (UNLIKELY((uint64_t)key >= (uint64_t)m_size)) {
      throwOOB(key);
      return;
    }
    mutate();
    tvRefcountedIncRef(val);
    TypedValue* tv = &m_data[key];
    tvRefcountedDecRef(tv);
    tvCopy(*val, *tv);
  }

  void resize(int64_t sz, TypedValue* val);

  enum SortFlavor { IntegerSort, StringSort, GenericSort };

  void sort(int sort_flags, bool ascending);
  bool usort(CVarRef cmp_function);

  static c_Vector* Clone(ObjectData* obj) {
    return BaseVector::Clone<c_Vector>(obj);
  }

  static void OffsetSet(ObjectData* obj, TypedValue* key, TypedValue* val);
  static void OffsetUnset(ObjectData* obj, TypedValue* key);

  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type) {
    BaseVector::Unserialize("Vector", obj, uns, sz, type);
  }

 private:
  template <typename AccessorT>
  SortFlavor preSort(const AccessorT& acc);

  void initFvFields(c_FrozenVector* fv);

  // Friends

  friend ObjectData* collectionDeepCopyVector(c_Vector* vec);
  friend void collectionAppend(ObjectData* obj, TypedValue* val);
  friend void triggerCow(c_Vector* vec);

  friend class c_Map;
  friend class c_StableMap;
  friend class c_Pair;
  friend class ArrayIter;
};

///////////////////////////////////////////////////////////////////////////////
// class VectorIterator

FORWARD_DECLARE_CLASS(VectorIterator);
class c_VectorIterator : public ExtObjectData {
 public:
  DECLARE_CLASS_NO_SWEEP(VectorIterator)

 public:
  explicit c_VectorIterator(Class* cls = c_VectorIterator::classof());
  ~c_VectorIterator();
  void t___construct();
  Variant t_current();
  Variant t_key();
  bool t_valid();
  void t_next();
  void t_rewind();

 private:
  SmartPtr<BaseVector> m_obj;
  ssize_t m_pos;
  int32_t m_version;

  friend class BaseVector;
};

///////////////////////////////////////////////////////////////////////////////
// class FrozenVector

FORWARD_DECLARE_CLASS(FrozenVector);
class c_FrozenVector : public BaseVector {
public:
  DECLARE_CLASS_NO_SWEEP(FrozenVector)

public:
  // The methods that implement the ConstVector interface simply forward
  // invocations to the implementations in BaseVector. Unfortunately, we need
  // to explicitly declare them so that the code automatically generated from
  // the IDL can link against them.

  // ConstCollection
  bool t_isempty();
  int64_t t_count();
  Object t_items();

  // ConstIndexAccess
  bool t_containskey(CVarRef key);
  Variant t_at(CVarRef key);
  Variant t_get(CVarRef key);

  // KeyedIterable
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_mapwithkey(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_filterwithkey(CVarRef callback);
  Object t_zip(CVarRef iterable);
  Object t_kvzip();
  Object t_keys();

  // Others
  void t___construct(CVarRef iterable = null_variant);
  Object t_lazy();
  Array t_toarray();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  int64_t t_linearsearch(CVarRef search_value);
  Object t_values();


  static Object ti_slice(CVarRef vec, CVarRef offset,
                         CVarRef len = uninit_null());

  static c_FrozenVector* Clone(ObjectData* obj) {
    return BaseVector::Clone<c_FrozenVector>(obj);
  }

  // Magic methods
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);

  // Materialization methods
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_tomap();
  Object t_tostablemap();
  Object t_toset();

public:

  explicit c_FrozenVector(Class* cls = c_FrozenVector::classof());

  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type) {
    BaseVector::Unserialize("FrozenVector", obj, uns, sz, type);
  }

  friend class c_Vector;
};

///////////////////////////////////////////////////////////////////////////////
// class Map

FORWARD_DECLARE_CLASS(Map);
class c_Map : public ExtObjectDataFlags<ObjectData::MapAttrInit|
                                        ObjectData::UseGet|
                                        ObjectData::UseSet|
                                        ObjectData::UseIsset|
                                        ObjectData::UseUnset|
                                        ObjectData::CallToImpl|
                                        ObjectData::HasClone> {
 public:
  DECLARE_CLASS_NO_SWEEP(Map)

 public:
  explicit c_Map(Class* cls = c_Map::classof());
  ~c_Map();
  void freeData();
  void t___construct(CVarRef iterable = null_variant);
  Object t_add(CVarRef val);
  Object t_addall(CVarRef val);
  Object t_clear();
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_keys();
  Object t_lazy();
  Object t_kvzip();
  Variant t_at(CVarRef key);
  Variant t_get(CVarRef key);
  Object t_set(CVarRef key, CVarRef value);
  Object t_setall(CVarRef iterable);
  Object t_put(CVarRef key, CVarRef value); // deprecated
  bool t_contains(CVarRef key);
  bool t_containskey(CVarRef key);
  Object t_remove(CVarRef key);
  Object t_removekey(CVarRef key);
  Array t_toarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_tomap();
  Object t_tostablemap();
  Object t_toset();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  Object t_values();
  Object t_differencebykey(CVarRef it);
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_mapwithkey(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_filterwithkey(CVarRef callback);
  Object t_zip(CVarRef iterable);
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);
  static Object ti_fromitems(CVarRef iterable);
  static Object ti_fromarray(CVarRef arr); // deprecated

  static void throwOOB(int64_t key) ATTRIBUTE_NORETURN;
  static void throwOOB(StringData* key) ATTRIBUTE_NORETURN;

  void init(CVarRef t);

  TypedValue* at(int64_t key) const {
    Bucket* p = find(key);
    if (LIKELY(p != NULL)) return &p->data;
    throwOOB(key);
    return NULL;
  }
  TypedValue* get(int64_t key) const {
    Bucket* p = find(key);
    if (p) return &p->data;
    return NULL;
  }
  TypedValue* at(StringData* key) const {
    Bucket* p = find(key->data(), key->size(), key->hash());
    if (LIKELY(p != NULL)) return &p->data;
    throwOOB(key);
    return NULL;
  }
  TypedValue* get(StringData* key) const {
    Bucket* p = find(key->data(), key->size(), key->hash());
    if (p) return &p->data;
    return NULL;
  }
  void set(int64_t key, TypedValue* val) {
    assert(val->m_type != KindOfRef);
    update(key, val);
  }
  void set(StringData* key, TypedValue* val) {
    assert(val->m_type != KindOfRef);
    update(key, val);
  }
  void add(TypedValue* val);
  void remove(int64_t key) {
    ++m_version;
    erase(find(key));
  }
  void remove(StringData* key) {
    ++m_version;
    erase(find(key->data(), key->size(), key->hash()));
  }
  bool contains(int64_t key) const {
    return find(key);
  }
  bool contains(StringData* key) const {
    return find(key->data(), key->size(), key->hash());
  }
  void reserve(int64_t sz) {
    if (int64_t(m_load) + sz - int64_t(m_size) >= computeMaxLoad()) {
      adjustCapacityImpl(sz);
    }
  }
  int getVersion() const {
    return m_version;
  }
  int64_t size() const {
    return m_size;
  }
  Array toArrayImpl() const;
  bool toBoolImpl() const {
    return (m_size != 0);
  }

  static c_Map* Clone(ObjectData* obj);
  static Array ToArray(const ObjectData* obj);
  static bool ToBool(const ObjectData* obj);
  static TypedValue* OffsetGet(ObjectData* obj, TypedValue* key);
  static void OffsetSet(ObjectData* obj, TypedValue* key, TypedValue* val);
  static bool OffsetIsset(ObjectData* obj, TypedValue* key);
  static bool OffsetEmpty(ObjectData* obj, TypedValue* key);
  static bool OffsetContains(ObjectData* obj, TypedValue* key);
  static void OffsetUnset(ObjectData* obj, TypedValue* key);
  static bool Equals(const ObjectData* obj1, const ObjectData* obj2);
  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type);

  static uint sizeOffset() { return offsetof(c_Map, m_size); }

  static const int32_t KindOfTombstone = -1;

  struct Bucket {
    /**
     * Buckets are 24 bytes and we allocate Buckets continguously in memory
     * without any padding, so some Buckets span multiple cache lines. We
     * access data.m_aux, data.m_type, and ikey/skey during hash lookup, so we
     * intentionally put the data field first so that the accessed fields are
     * all next to each other, which means that they will be on the same cache
     * line for 87.5% of the buckets.
     *
     * The key is either a string pointer or an int value, and the
     * m_aux.u_hash field in data is used to discriminate the key type.
     * u_hash = 0 means int, nonzero values contain 31 bits of a string's
     * hashcode. It is critical that when we return &data to clients, that
     * they not read or write the m_aux field.
     */
    TypedValueAux data;
    union {
      int64_t ikey;
      StringData *skey;
    };
    inline bool hasStrKey() const { return data.hash() != 0; }
    inline bool hasIntKey() const { return data.hash() == 0; }
    inline void setStrKey(StringData* k, strhash_t h) {
      skey = k;
      skey->incRefCount();
      data.hash() = int32_t(h) | 0x80000000;
    }
    inline void setIntKey(int64_t k) {
      ikey = k;
      data.hash() = 0;
    }
    inline int64_t hashKey() const {
      return data.hash() == 0 ? ikey : data.hash();
    }
    inline int32_t hash() const {
      return data.hash();
    }
    bool validValue() const {
      return (intptr_t(data.m_type) > 0);
    }
    bool empty() const {
      return data.m_type == KindOfUninit;
    }
    bool tombstone() const {
      return data.m_type == KindOfTombstone;
    }
    void dump();
  };

 private:
  /**
   * Map uses a power of two for the table size and quadratic probing to
   * resolve hash collisions.
   *
   * When an element is removed from the table, a marker called a "tombstone"
   * is left behind in the slot that the element used to occupy. The tombstone
   * will remain in that slot until either (a) the table is resized, or (b) a
   * new element is inserted into that slot.
   *
   * To ensure that hash lookups are efficient, Map keeps the load factor
   * of the table below 75%. If adding a new element causes the load to
   * increase to 75% or greater, we grow the table to lower the load. Note
   * that tombstones count towards load.
   *
   * To ensure that iteration performance is efficient, Map keeps the ratio
   * of # elements / # slots to be at least 18.75%. If removing an element
   * causes the ratio to drop below 18.75%, we shrink the table to increase
   * the ratio.
   *
   * When a Map has never had any removals performed, the load factor is
   * guaranteed to be between 37.5% and 75% (as long as the Map has at least
   * 2 elements).
   */

  uint m_size;
  Bucket* m_data;
  uint m_load;
  uint m_nLastSlot;
  int32_t m_version;

  size_t numSlots() const {
    return m_nLastSlot + 1;
  }

  // The maximum load factor is 75%.
  size_t computeMaxLoad() const {
    size_t n = numSlots();
    return (n - (n >> 2));
  }

  // When the map is not empty, the minimum allowed ratio
  // of # elements / # slots is 18.75%.
  size_t computeMinElements() const {
    size_t n = numSlots();
    return ((n >> 3) + ((n+8) >> 4));
  }

  // We use this funny-looking helper to make g++ use lea and shl
  // instructions instead of imul when indexing into m_data
  Bucket* fetchBucket(Bucket* data, intptr_t slot) const {
    assert(sizeof(Bucket) == 24);
    assert(sizeof(int64_t) == 8);
    assert(slot >= 0 && slot <= m_nLastSlot);
    intptr_t index = slot + (slot<<1);
    int64_t* ptr = (int64_t*)data;
    return (Bucket*)(&ptr[index]);
  }

  Bucket* fetchBucket(intptr_t slot) const {
    return fetchBucket(m_data, slot);
  }

  Bucket* find(int64_t h) const;
  Bucket* find(const char* k, int len, strhash_t prehash) const;
  Bucket* findForInsert(int64_t h) const;
  Bucket* findForInsert(const char* k, int len, strhash_t prehash) const;
  Bucket* findForNewInsert(size_t h0) const;

  bool update(int64_t h, TypedValue* data);
  bool update(StringData* key, TypedValue* data);
  void erase(Bucket* prev);

  void adjustCapacityImpl(int64_t sz);
  void adjustCapacity() {
    adjustCapacityImpl(m_size);
  }

  void deleteBuckets();

  ssize_t iter_begin() const {
    if (!m_size) return 0;
    for (uint i = 0; i <= m_nLastSlot; ++i) {
      Bucket* p = fetchBucket(i);
      if (p->validValue()) {
        return reinterpret_cast<ssize_t>(p);
      }
    }
    return 0;
  }
  ssize_t iter_next(ssize_t prev) const;
  ssize_t iter_prev(ssize_t prev) const;
  Variant iter_key(ssize_t pos) const;
  TypedValue* iter_value(ssize_t pos) const;

  static void throwBadKeyType() ATTRIBUTE_NORETURN;

  friend ObjectData* collectionDeepCopyMap(c_Map* mp);
  friend class c_MapIterator;
  friend class c_Vector;
  friend class c_StableMap;
  friend class ArrayIter;
  friend class c_GenMapWaitHandle;

  static void compileTimeAssertions() {
    // For performance, all native collection classes have their m_size field
    // at the same offset.
    static_assert(offsetof(c_Map, m_size) == FAST_COLLECTION_SIZE_OFFSET, "");
  }
};

///////////////////////////////////////////////////////////////////////////////
// class MapIterator

FORWARD_DECLARE_CLASS(MapIterator);
class c_MapIterator : public ExtObjectData {
 public:
  DECLARE_CLASS_NO_SWEEP(MapIterator)

 public:
  explicit c_MapIterator(Class* cls = c_MapIterator::classof());
  ~c_MapIterator();
  void t___construct();
  Variant t_current();
  Variant t_key();
  bool t_valid();
  void t_next();
  void t_rewind();

 private:
  SmartPtr<c_Map> m_obj;
  ssize_t m_pos;
  int32_t m_version;

  friend class c_Map;
};

///////////////////////////////////////////////////////////////////////////////
// class StableMap

FORWARD_DECLARE_CLASS(StableMap);
class c_StableMap : public ExtObjectDataFlags<ObjectData::StableMapAttrInit|
                                              ObjectData::UseGet|
                                              ObjectData::UseSet|
                                              ObjectData::UseIsset|
                                              ObjectData::UseUnset|
                                              ObjectData::CallToImpl|
                                              ObjectData::HasClone> {
 public:
  DECLARE_CLASS_NO_SWEEP(StableMap)

 public:
  explicit c_StableMap(Class* cls = c_StableMap::classof());
  ~c_StableMap();
  void freeData();
  void t___construct(CVarRef iterable = null_variant);
  Object t_add(CVarRef val);
  Object t_addall(CVarRef val);
  Object t_clear();
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_keys();
  Object t_lazy();
  Object t_kvzip();
  Variant t_at(CVarRef key);
  Variant t_get(CVarRef key);
  Object t_set(CVarRef key, CVarRef value);
  Object t_setall(CVarRef iterable);
  Object t_put(CVarRef key, CVarRef value); // deprecated
  bool t_contains(CVarRef key);
  bool t_containskey(CVarRef key);
  Object t_remove(CVarRef key);
  Object t_removekey(CVarRef key);
  Array t_toarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_tomap();
  Object t_tostablemap();
  Object t_toset();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  Object t_values();
  Object t_differencebykey(CVarRef it);
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_mapwithkey(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_filterwithkey(CVarRef callback);
  Object t_zip(CVarRef iterable);
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);
  static Object ti_fromitems(CVarRef iterable);
  static Object ti_fromarray(CVarRef arr); // deprecated

  static void throwOOB(int64_t key) ATTRIBUTE_NORETURN;
  static void throwOOB(StringData* key) ATTRIBUTE_NORETURN;

  void init(CVarRef t);

  TypedValue* at(int64_t key) {
    Bucket* p = find(key);
    if (LIKELY(p != NULL)) return &p->data;
    throwOOB(key);
    return NULL;
  }
  TypedValue* at(StringData* key) {
    Bucket* p = find(key->data(), key->size(), key->hash());
    if (LIKELY(p != NULL)) return &p->data;
    throwOOB(key);
    return NULL;
  }
  TypedValue* get(int64_t key) {
    Bucket* p = find(key);
    if (p != NULL) return &p->data;
    return NULL;
  }
  TypedValue* get(StringData* key) {
    Bucket* p = find(key->data(), key->size(), key->hash());
    if (p != NULL) return &p->data;
    return NULL;
  }
  void set(int64_t key, TypedValue* val) {
    update(key, val);
  }
  void set(StringData* key, TypedValue* val) {
    update(key, val);
  }
  void add(TypedValue* val);
  void remove(int64_t key) {
    ++m_version;
    erase(findForErase(key));
  }
  void remove(StringData* key) {
    ++m_version;
    erase(findForErase(key->data(), key->size(), key->hash()));
  }
  bool contains(int64_t key) {
    return find(key);
  }
  bool contains(StringData* key) {
    return find(key->data(), key->size(), key->hash());
  }
  void reserve(int64_t sz) {
    if (sz > int64_t(m_nTableSize)) {
      adjustCapacityImpl(sz);
    }
  }
  int getVersion() const {
    return m_version;
  }
  int64_t size() const {
    return m_size;
  }
  Array toArrayImpl() const;
  bool toBoolImpl() const {
    return (m_size != 0);
  }

  static c_StableMap* Clone(ObjectData* obj);
  static Array ToArray(const ObjectData* obj);
  static bool ToBool(const ObjectData* obj);
  static TypedValue* OffsetGet(ObjectData* obj, TypedValue* key);
  static void OffsetSet(ObjectData* obj, TypedValue* key, TypedValue* val);
  static bool OffsetIsset(ObjectData* obj, TypedValue* key);
  static bool OffsetEmpty(ObjectData* obj, TypedValue* key);
  static bool OffsetContains(ObjectData* obj, TypedValue* key);
  static void OffsetUnset(ObjectData* obj, TypedValue* key);
  static bool Equals(const ObjectData* obj1, const ObjectData* obj2);
  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type);

  static uint sizeOffset() { return offsetof(c_StableMap, m_size); }

  struct Bucket {
    Bucket() : ikey(0), pListNext(nullptr), pListLast(nullptr), pNext(nullptr) {
      data.hash() = 0;
    }
    explicit Bucket(TypedValue* tv) : ikey(0), pListNext(nullptr),
        pListLast(nullptr), pNext(nullptr) {
      assert(tv->m_type != KindOfRef);
      cellDup(*tv, data);
      data.hash() = 0;
    }
    ~Bucket();

    template<class... Args>
    static Bucket* Make(Args&&... args) {
      return new (MM().smartMallocSizeLogged(sizeof(Bucket)))
        Bucket(std::forward<Args>(args)...);
    }
    void release();

    // set the top bit for string hashes to make sure the hash
    // value is never zero. hash value 0 corresponds to integer key.
    static inline int32_t encodeHash(strhash_t h) {
      return int32_t(h) | 0x80000000;
    }

    /* The key is either a string pointer or an int value, and the m_aux.u_hash
     * field in data is used to discriminate the key type. u_hash = 0 means
     * int, nonzero values contain 31 bits of a string's hashcode.
     * It is critical that when we return &data to clients, that they not
     * read or write the m_aux field! */
    TypedValueAux data;
    union {
      int64_t ikey;
      StringData* skey;
    };
    Bucket* pListNext;
    Bucket* pListLast;
    Bucket* pNext;

    inline bool hasStrKey() const { return data.hash() != 0; }
    inline bool hasIntKey() const { return data.hash() == 0; }
    inline void setStrKey(StringData* k, strhash_t h) {
      skey = k;
      skey->incRefCount();
      data.hash() = encodeHash(h);
    }
    inline void setIntKey(int64_t k) {
      ikey = k;
      data.hash() = 0;
    }
    inline int64_t hashKey() const {
      return data.hash() == 0 ? ikey : data.hash();
    }
    inline int32_t hash() const {
      return data.hash();
    }

    void dump();
  };

  enum SortFlavor { IntegerSort, StringSort, GenericSort };

 private:
  template <typename AccessorT>
  SortFlavor preSort(Bucket** buffer, const AccessorT& acc, bool checkTypes);

  void postSort(Bucket** buffer);

 public:
  void asort(int sort_flags, bool ascending);
  void ksort(int sort_flags, bool ascending);
  bool uasort(CVarRef cmp_function);
  bool uksort(CVarRef cmp_function);

 private:
  uint m_size;
  uint m_nTableSize;
  uint m_nTableMask;
  int32_t m_version;
  Bucket* m_pListHead;
  Bucket* m_pListTail;
  Bucket** m_arBuckets;

  Bucket* find(int64_t h) const;
  Bucket* find(const char* k, int len, strhash_t prehash) const;
  Bucket** findForErase(int64_t h) const;
  Bucket** findForErase(const char* k, int len, strhash_t prehash) const;

  bool update(int64_t h, TypedValue* data);
  bool update(StringData* key, TypedValue* data);
  void erase(Bucket** prev);

  void adjustCapacityImpl(int64_t sz);
  void adjustCapacity() {
    adjustCapacityImpl(m_size);
  }

  void deleteBuckets();

  ssize_t iter_begin() const {
    Bucket* p = m_pListHead;
    return reinterpret_cast<ssize_t>(p);
  }
  ssize_t iter_next(ssize_t prev) const;
  ssize_t iter_prev(ssize_t prev) const;
  Variant iter_key(ssize_t pos) const;
  TypedValue* iter_value(ssize_t pos) const;

  static void throwBadKeyType() ATTRIBUTE_NORETURN;

  friend ObjectData* collectionDeepCopyStableMap(c_StableMap* smp);
  friend class c_StableMapIterator;
  friend class c_Vector;
  friend class c_Map;
  friend class ArrayIter;

  static void compileTimeAssertions() {
    // For performance, all native collection classes have their m_size field
    // at the same offset.
    static_assert(
      offsetof(c_StableMap, m_size) == FAST_COLLECTION_SIZE_OFFSET, "");
  }
};

///////////////////////////////////////////////////////////////////////////////
// class StableMapIterator

FORWARD_DECLARE_CLASS(StableMapIterator);
class c_StableMapIterator : public ExtObjectData {
 public:
  DECLARE_CLASS_NO_SWEEP(StableMapIterator)

 public:
  explicit c_StableMapIterator(Class* cls = c_StableMapIterator::classof());
  ~c_StableMapIterator();
  void t___construct();
  Variant t_current();
  Variant t_key();
  bool t_valid();
  void t_next();
  void t_rewind();

 private:
  SmartPtr<c_StableMap> m_obj;
  ssize_t m_pos;
  int32_t m_version;

  friend class c_StableMap;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * BaseSet is a hash-table implementation of the Set ADT. It doesn't represent
 * any PHP-land class. That job is delegated to its child classes.
 *
 * BaseSet uses a power of two for the table size and quadratic probing to
 * resolve hash collisions, similar to the Map class. See the comments
 * in the Map class for more details on how the hash table works and how
 * we decide when to grow or shrink the table.
 */
class BaseSet : public ExtObjectData {

public:
  // Inner types and constants.

  static const int32_t KindOfTombstone = -1;
  static const char emptySetSlot[];

  /**
   * A hash-table bucket.
   */
  struct Bucket {

    inline bool hasStr() const { return IS_STRING_TYPE(data.m_type); }
    inline bool hasInt() const { return data.m_type == KindOfInt64; }

    inline void setStr(StringData* k, strhash_t h) {
      k->incRefCount();
      data.m_data.pstr = k;
      data.m_type = KindOfString;
      data.hash() = int32_t(h) | 0x80000000;
    }

    inline void setInt(int64_t k) {
      data.m_data.num = k;
      data.m_type = KindOfInt64;
      data.hash() = int32_t(k) | 0x80000000;
    }

    inline int32_t hash() const { return data.hash(); }
    bool validValue() const { return (intptr_t(data.m_type) > 0); }
    bool empty() const { return data.m_type == KindOfUninit; }
    bool tombstone() const { return data.m_type == KindOfTombstone; }

    void dump() {
      if (!validValue()) {
        printf("BaseSet::Bucket: %s\n", (empty() ? "empty" : "tombstone"));
        return;
      }
      printf("BaseSet::Bucket: %d\n", hash());
      tvAsCVarRef(&data).dump();
    }

    // Buckets are 16 bytes. We use m_aux for our own nefarious purposes.
    // It is critical that when we return &data to clients, that they not
    // read or write the m_aux field.
    TypedValueAux data;
  };

public:
  // API

  void init(CVarRef t);

  void add(TypedValue* val) {
    if (val->m_type == KindOfInt64) {
      add(val->m_data.num);
    } else if (IS_STRING_TYPE(val->m_type)) {
      add(val->m_data.pstr);
    } else {
      throwBadValueType();
    }
  }

  void add(int64_t h);
  void add(StringData* key);

  void remove(int64_t key) {
    ++m_version;
    erase(find(key));
  }

  void remove(StringData* key) {
    ++m_version;
    erase(find(key->data(), key->size(), key->hash()));
  }

  bool contains(int64_t key) {
    return find(key);
  }

  bool contains(StringData* key) {
    return find(key->data(), key->size(), key->hash());
  }

  void reserve(int64_t sz) {
    if (int64_t(m_load) + sz - int64_t(m_size) >= computeMaxLoad()) {
      adjustCapacityImpl(sz);
    }
  }

  int getVersion() const { return m_version; }
  int64_t size() const { return m_size; }

  template<typename TSet>
  static TSet* Clone(ObjectData* obj) {
    auto thiz = static_cast<TSet*>(obj);
    auto target = static_cast<TSet*>(obj->cloneImpl());

    if (!thiz->m_size) return target;

    assert(thiz->m_nLastSlot != 0);
    target->m_size = thiz->m_size;
    target->m_load = thiz->m_load;
    target->m_nLastSlot = thiz->m_nLastSlot;
    target->m_data = (Bucket*)smart_malloc(thiz->numSlots() * sizeof(Bucket));
    memcpy(target->m_data, thiz->m_data, thiz->numSlots() * sizeof(Bucket));

    for (uint i = 0; i <= thiz->m_nLastSlot; ++i) {
      Bucket& p = thiz->m_data[i];
      if (p.validValue()) {
        tvRefcountedIncRef(&p.data);
      }
    }

    return target;
  }

  // Static methods

  static void throwOOB(int64_t key) ATTRIBUTE_NORETURN;
  static void throwOOB(StringData* key) ATTRIBUTE_NORETURN;
  static void throwNoIndexAccess() ATTRIBUTE_NORETURN;

  static Array ToArray(const ObjectData* obj);
  static bool ToBool(const ObjectData* obj);

  static TypedValue* OffsetGet(ObjectData* obj, TypedValue* key);
  static void OffsetSet(ObjectData* obj, TypedValue* key, TypedValue* val);
  static bool OffsetIsset(ObjectData* obj, TypedValue* key);
  static bool OffsetEmpty(ObjectData* obj, TypedValue* key);
  static bool OffsetContains(ObjectData* obj, TypedValue* key);
  static void OffsetUnset(ObjectData* obj, TypedValue* key);

  static bool Equals(const ObjectData* obj1, const ObjectData* obj2);

  static void Unserialize(const char* setType, ObjectData* obj,
                          VariableUnserializer* uns, int64_t sz, char type);

  static uint sizeOffset() { return offsetof(BaseSet, m_size); }

protected:
  // PHP-land methods exported by child classes.

  void    phpConstruct(CVarRef iterable = null_variant);

  Object  phpAdd(CVarRef val) {
    TypedValue* tv = cvarToCell(&val);
    add(tv);
    return this;
  }

  Object  phpAddAll(CVarRef val);

  Object  phpClear() {
    deleteBuckets();
    freeData();
    m_size = 0;
    m_load = 0;
    m_nLastSlot = 0;
    m_data = (Bucket*)emptySetSlot;
    return this;
  }

  bool    phpIsEmpty() { return !toBoolImpl(); }
  int64_t phpCount() { return m_size; }
  Object  phpItems() { return SystemLib::AllocLazyIterableViewObject(this); }

  template<typename TVector>
  Object  phpValues() {
    TVector* vec;
    Object o = vec = NEWOBJ(TVector)();
    vec->init(VarNR(this));
    return o;
  }

  Object  phpLazy() { return SystemLib::AllocLazyIterableViewObject(this); }
  bool    phpContains(CVarRef key);
  Object  phpRemove(CVarRef key);
  Array   phpToArray() { return toArrayImpl(); }
  Array   phpToKeysArray() { return phpToValuesArray(); }
  Array   phpToValuesArray();
  Object  phpGetIterator();

  template<typename TSet>
  Object phpMap(CVarRef callback) {
    CallCtx ctx;
    vm_decode_function(callback, nullptr, false, ctx);
    if (!ctx.func) {
      Object e(SystemLib::AllocInvalidArgumentExceptionObject(
        "Parameter must be a valid callback"));
      throw e;
    }
    TSet* st;
    Object obj = st = NEWOBJ(TSet)();
    if (!m_size) return obj;
    assert(m_nLastSlot != 0);
    st->m_size = m_size;
    st->m_load = m_load;
    st->m_nLastSlot = 0;
    st->m_data = (Bucket*)smart_malloc(numSlots() * sizeof(Bucket));
    // We need to zero out the first slot in case an exception
    // is thrown during the first iteration, because ~c_Set()
    // will decRef all slots up to (and including) m_nLastSlot.
    st->m_data[0].data.m_type = (DataType)0;
    uint nLastSlot = m_nLastSlot;
    for (uint i = 0; i <= nLastSlot; st->m_nLastSlot = i++) {
      Bucket& p = m_data[i];
      Bucket& np = st->m_data[i];
      if (!p.validValue()) {
        np.data.m_type = p.data.m_type;
        continue;
      }
      TypedValue* tv = &np.data;
      int32_t version = m_version;
      g_vmContext->invokeFuncFew(tv, ctx, 1, &p.data);
      if (UNLIKELY(version != m_version)) {
        tvRefcountedDecRef(tv);
        throw_collection_modified();
      }
      np.data.hash() = p.data.hash();
    }
    return obj;
  }

  template<typename TSet>
  Object phpFilter(CVarRef callback) {
    CallCtx ctx;
    vm_decode_function(callback, nullptr, false, ctx);
    if (!ctx.func) {
      Object e(SystemLib::AllocInvalidArgumentExceptionObject(
        "Parameter must be a valid callback"));
      throw e;
    }
    TSet* st;
    Object obj = st = NEWOBJ(TSet)();
    if (!m_size) return obj;
    uint nLastSlot = m_nLastSlot;
    for (uint i = 0; i <= nLastSlot; ++i) {
      Bucket& p = m_data[i];
      if (!p.validValue()) continue;
      Variant ret;
      int32_t version = m_version;
      g_vmContext->invokeFuncFew(ret.asTypedValue(), ctx, 1, &p.data);
      if (UNLIKELY(version != m_version)) {
        throw_collection_modified();
      }
      if (!ret.toBoolean()) continue;
      if (p.hasInt()) {
        st->add(p.data.m_data.num);
      } else {
        assert(p.hasStr());
        st->add(p.data.m_data.pstr);
      }
    }
    return obj;
  }

  template<typename TSet>
  Object phpZip(CVarRef iterable) {
    size_t sz;
    ArrayIter iter = getArrayIterHelper(iterable, sz);
    if (m_size && iter) {
      // At present, BaseSets only support int values and string values,
      // so if this BaseSet is non empty and the iterable is non empty
      // the zip operation will always fail
      throwBadValueType();
    }
    Object obj = NEWOBJ(TSet)();
    return obj;
  }

  template<typename TSet>
  static Object phpFromItems(CVarRef iterable) {
    if (iterable.isNull()) return NEWOBJ(TSet)();
    size_t sz;
    ArrayIter iter = getArrayIterHelper(iterable, sz);
    TSet* target;
    Object ret = target = NEWOBJ(TSet)();
    for (; iter; ++iter) {
      Variant v = iter.second();
      if (v.isInteger()) {
        target->add(v.toInt64());
      } else if (v.isString()) {
        target->add(v.getStringData());
      } else {
        throwBadValueType();
      }
    }
    return ret;
  }

  template<typename TSet>
  static Object phpFromArray(CVarRef arr) {
    if (!arr.isArray()) {
      Object e(SystemLib::AllocInvalidArgumentExceptionObject(
        "Parameter arr must be an array"));
      throw e;
    }
    TSet* st;
    Object ret = st = NEWOBJ(TSet)();
    ArrayData* ad = arr.getArrayData();
    for (ssize_t pos = ad->iter_begin(); pos != ArrayData::invalid_index;
         pos = ad->iter_advance(pos)) {
      CVarRef v = ad->getValueRef(pos);
      if (v.isInteger()) {
        st->add(v.toInt64());
      } else if (v.isString()) {
        st->add(v.getStringData());
      } else {
        throwBadValueType();
      }
    }
    return ret;
  }

  template<typename TSet>
  static Object phpFromArrays(int _argc, CArrRef _argv = null_array) {
    TSet* st;
    Object ret = st = NEWOBJ(TSet)();
    for (ArrayIter iter(_argv); iter; ++iter) {
      Variant arr = iter.second();
      if (!arr.isArray()) {
        Object e(SystemLib::AllocInvalidArgumentExceptionObject(
          "Parameters must be arrays"));
        throw e;
      }
      ArrayData* ad = arr.getArrayData();
      for (ssize_t pos = ad->iter_begin(); pos != ArrayData::invalid_index;
           pos = ad->iter_advance(pos)) {
        st->phpAdd(ad->getValueRef(pos));
      }
    }
    return ret;
  }

protected:
  // BaseSet is an abstract class.

  explicit BaseSet(Class* cls);
  /* virtual */ ~BaseSet();

private:
  // Helpers

  Array toArrayImpl() const;
  bool toBoolImpl() const { return (m_size != 0); }
  size_t numSlots() const { return m_nLastSlot + 1; }

  void freeData() {
    if (m_data != (Bucket*)emptySetSlot) {
      smart_free(m_data);
    }
    m_data = (Bucket*)emptySetSlot;
  }

  // The maximum load factor is 75%.
  size_t computeMaxLoad() const {
    size_t n = numSlots();
    return (n - (n >> 2));
  }

  // When the map is not empty, the minimum allowed ratio
  // of # elements / # slots is 18.75%.
  size_t computeMinElements() const {
    size_t n = numSlots();
    return ((n >> 3) + ((n+8) >> 4));
  }

  Bucket* fetchBucket(Bucket* data, intptr_t slot) const {
    return &data[slot];
  }

  Bucket* fetchBucket(intptr_t slot) const {
    return fetchBucket(m_data, slot);
  }

  Bucket* find(int64_t h) const;
  Bucket* find(const char* k, int len, strhash_t prehash) const;
  Bucket* findForInsert(int64_t h) const;
  Bucket* findForInsert(const char* k, int len, strhash_t prehash) const;
  Bucket* findForNewInsert(size_t h0) const;

  void erase(Bucket* prev);
  void adjustCapacity() { adjustCapacityImpl(m_size); }
  void adjustCapacityImpl(int64_t sz);

  void deleteBuckets();

  ssize_t iter_begin() const {
    if (!m_size) return 0;
    for (uint i = 0; i <= m_nLastSlot; ++i) {
      Bucket* p = fetchBucket(i);
      if (p->validValue()) {
        return reinterpret_cast<ssize_t>(p);
      }
    }
    return 0;
  }

  ssize_t iter_next(ssize_t prev) const;
  ssize_t iter_prev(ssize_t prev) const;
  const TypedValue* iter_value(ssize_t pos) const;
  Variant iter_key(ssize_t pos) const { return uninit_null(); }

  static void throwBadValueType() ATTRIBUTE_NORETURN;

 private:
  // Internal state.

  uint m_size;
  Bucket* m_data;
  uint m_load;
  uint m_nLastSlot;
  int32_t m_version;

  friend ObjectData* collectionDeepCopySet(c_Set* st);
  friend class c_SetIterator;
  friend class c_Vector;
  friend class c_Map;
  friend class c_StableMap;
  friend class ArrayIter;

  static void compileTimeAssertions() {
    // For performance, all native collection classes have their m_size field
    // at the same offset.
    static_assert(offsetof(BaseSet, m_size) == FAST_COLLECTION_SIZE_OFFSET, "");
  }
};

///////////////////////////////////////////////////////////////////////////////
// class Set

FORWARD_DECLARE_CLASS(Set);
class c_Set : public BaseSet {

 public:
  DECLARE_CLASS_NO_SWEEP(Set)

 public:
  // PHP-land methods.

  explicit c_Set(Class* cls = c_Set::classof());
  void t___construct(CVarRef iterable = null_variant);
  Object t_add(CVarRef val);
  Object t_addall(CVarRef val);
  Object t_clear();
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_values();
  Object t_lazy();
  bool t_contains(CVarRef key);
  Object t_remove(CVarRef key);
  Array t_toarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_toset();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_zip(CVarRef iterable);
  Object t_difference(CVarRef iterable);
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);
  static Object ti_fromitems(CVarRef iterable);
  static Object ti_fromarray(CVarRef arr); // deprecated
  static Object ti_fromarrays(int _argc, CArrRef _argv = null_array);

 public:

  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type);

  static c_Set* Clone(ObjectData* obj);
};

///////////////////////////////////////////////////////////////////////////////
// class FrozenSet

FORWARD_DECLARE_CLASS(FrozenSet);
class c_FrozenSet : public BaseSet {

 public:
  DECLARE_CLASS_NO_SWEEP(FrozenSet)

 public:
  // PHP-land methods.

  void t___construct(CVarRef iterable = null_variant);

  // API
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_values();
  Object t_lazy();
  bool t_contains(CVarRef key);
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_zip(CVarRef iterable);

  // Materialization methods.
  Array t_toarray();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_toset();

  // Magic methods.
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);

  // Static methods.
  static Object ti_fromitems(CVarRef iterable);
  static Object ti_fromarrays(int _argc, CArrRef _argv = null_array);

 public:
  explicit c_FrozenSet(Class* cls = c_FrozenSet::classof());

  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type);

  static c_FrozenSet* Clone(ObjectData* obj);
};

///////////////////////////////////////////////////////////////////////////////
// class SetIterator

FORWARD_DECLARE_CLASS(SetIterator);
class c_SetIterator : public ExtObjectData {
 public:
  DECLARE_CLASS_NO_SWEEP(SetIterator)

 public:
  explicit c_SetIterator(Class* cls = c_SetIterator::classof());
  ~c_SetIterator();
  void t___construct();
  Variant t_current();
  Variant t_key();
  bool t_valid();
  void t_next();
  void t_rewind();

 private:
  SmartPtr<BaseSet> m_obj;
  ssize_t m_pos;
  int32_t m_version;

  friend class BaseSet;
};

///////////////////////////////////////////////////////////////////////////////
// class Pair

FORWARD_DECLARE_CLASS(Pair);
class c_Pair : public ExtObjectDataFlags<ObjectData::PairAttrInit|
                                         ObjectData::UseGet|
                                         ObjectData::UseSet|
                                         ObjectData::UseIsset|
                                         ObjectData::UseUnset|
                                         ObjectData::HasClone> {
 public:
  DECLARE_CLASS_NO_SWEEP(Pair)

 public:
  explicit c_Pair(Class* cls = c_Pair::classof());
  ~c_Pair();
  void t___construct();
  bool t_isempty();
  int64_t t_count();
  Object t_items();
  Object t_keys();
  Object t_values();
  Object t_lazy();
  Object t_kvzip();
  Variant t_at(CVarRef key);
  Variant t_get(CVarRef key);
  bool t_containskey(CVarRef key);
  Array t_toarray();
  Object t_tovector();
  Object t_tofrozenvector();
  Object t_tomap();
  Object t_tostablemap();
  Object t_toset();
  Array t_tokeysarray();
  Array t_tovaluesarray();
  Object t_getiterator();
  Object t_map(CVarRef callback);
  Object t_mapwithkey(CVarRef callback);
  Object t_filter(CVarRef callback);
  Object t_filterwithkey(CVarRef callback);
  Object t_zip(CVarRef iterable);
  String t___tostring();
  Variant t___get(Variant name);
  Variant t___set(Variant name, Variant value);
  bool t___isset(Variant name);
  Variant t___unset(Variant name);

  static void throwOOB(int64_t key) ATTRIBUTE_NORETURN;

  /**
   * Most methods that operate on Pairs can safely assume that all Pairs have
   * two elements that have been initialized. However, methods that deal with
   * initializing and destructing Pairs needs to handle intermediate states
   * where one or both of the elements is uninitialized.
   */
  bool isFullyConstructed() const {
    return m_size == 2;
  }

  TypedValue* at(int64_t key) {
    assert(isFullyConstructed());
    if (UNLIKELY(uint64_t(key) >= uint64_t(2))) {
      throwOOB(key);
      return NULL;
    }
    return &getElms()[key];
  }
  TypedValue* get(int64_t key) {
    assert(isFullyConstructed());
    if (uint64_t(key) >= uint64_t(2)) {
      return NULL;
    }
    return &getElms()[key];
  }
  void initAdd(TypedValue* val) {
    assert(!isFullyConstructed());
    assert(val->m_type != KindOfRef);
    cellDup(*val, getElms()[m_size]);
    ++m_size;
  }
  bool contains(int64_t key) {
    assert(isFullyConstructed());
    return (uint64_t(key) < uint64_t(2));
  }

  Array toArrayImpl() const;

  static c_Pair* Clone(ObjectData* obj);
  static Array ToArray(const ObjectData* obj);
  static TypedValue* OffsetGet(ObjectData* obj, TypedValue* key);
  static void OffsetSet(ObjectData* obj, TypedValue* key, TypedValue* val);
  static bool OffsetIsset(ObjectData* obj, TypedValue* key);
  static bool OffsetEmpty(ObjectData* obj, TypedValue* key);
  static bool OffsetContains(ObjectData* obj, TypedValue* key);
  static void OffsetUnset(ObjectData* obj, TypedValue* key);
  static bool Equals(const ObjectData* obj1, const ObjectData* obj2);
  static void Unserialize(ObjectData* obj, VariableUnserializer* uns,
                          int64_t sz, char type);

  int64_t size() const {
    assert(isFullyConstructed());
    return 2;
  }

  static uint dataOffset() { return offsetof(c_Pair, elm0); }

 private:
  static void throwBadKeyType() ATTRIBUTE_NORETURN;

  uint m_size;

  // TODO Can we add something here to make sure elm0 is 16-byte aligned?
  TypedValue elm0;
  TypedValue elm1;

  TypedValue* getElms() const {
    return (TypedValue*)(&elm0);
  }

  friend ObjectData* collectionDeepCopyPair(c_Pair* pair);
  friend class c_PairIterator;
  friend class c_Vector;
  friend class BaseVector;
  friend class c_Map;
  friend class c_StableMap;
  friend class ArrayIter;

  static void compileTimeAssertions() {
    // For performance, all native collection classes have their m_size field
    // at the same offset.
    static_assert(offsetof(c_Pair, m_size) == FAST_COLLECTION_SIZE_OFFSET, "");
  }
};

///////////////////////////////////////////////////////////////////////////////
// class PairIterator

FORWARD_DECLARE_CLASS(PairIterator);
class c_PairIterator : public ExtObjectData {
 public:
  DECLARE_CLASS_NO_SWEEP(PairIterator)

 public:
  explicit c_PairIterator(Class* cls = c_PairIterator::classof());
  ~c_PairIterator();
  void t___construct();
  Variant t_current();
  Variant t_key();
  bool t_valid();
  void t_next();
  void t_rewind();

 private:
  SmartPtr<c_Pair> m_obj;
  ssize_t m_pos;

  friend class c_Pair;
};

///////////////////////////////////////////////////////////////////////////////

TypedValue* collectionGet(ObjectData* obj, TypedValue* key);
void collectionSet(ObjectData* obj, TypedValue* key, TypedValue* val);
bool collectionIsset(ObjectData* obj, TypedValue* key);
bool collectionEmpty(ObjectData* obj, TypedValue* key);
void collectionUnset(ObjectData* obj, TypedValue* key);
void collectionAppend(ObjectData* obj, TypedValue* val);
void collectionInitAppend(ObjectData* obj, TypedValue* val);
Variant& collectionOffsetGet(ObjectData* obj, int64_t offset);
Variant& collectionOffsetGet(ObjectData* obj, const String& offset);
Variant& collectionOffsetGet(ObjectData* obj, CVarRef offset);
void collectionOffsetSet(ObjectData* obj, int64_t offset, CVarRef val);
void collectionOffsetSet(ObjectData* obj, const String& offset, CVarRef val);
void collectionOffsetSet(ObjectData* obj, CVarRef offset, CVarRef val);
bool collectionOffsetContains(ObjectData* obj, CVarRef offset);
void collectionReserve(ObjectData* obj, int64_t sz);
void collectionUnserialize(ObjectData* obj, VariableUnserializer* uns,
                           int64_t sz, char type);
bool collectionEquals(const ObjectData* obj1, const ObjectData* obj2);
void collectionDeepCopyTV(TypedValue* tv);
ArrayData* collectionDeepCopyArray(ArrayData* arr);
ObjectData* collectionDeepCopyVector(c_Vector* vec);
ObjectData* collectionDeepCopyMap(c_Map* mp);
ObjectData* collectionDeepCopyStableMap(c_StableMap* smp);
ObjectData* collectionDeepCopySet(c_Set* st);
ObjectData* collectionDeepCopyPair(c_Pair* pair);

ObjectData* newCollectionHelper(uint32_t type, uint32_t size);

///////////////////////////////////////////////////////////////////////////////

inline TypedValue* cvarToCell(const Variant* v) {
  return const_cast<TypedValue*>(v->asCell());
}

inline Variant& collectionOffsetGet(ObjectData* obj, bool offset) {
  return collectionOffsetGet(obj, Variant(offset));
}

inline Variant& collectionOffsetGet(ObjectData* obj, double offset) {
  return collectionOffsetGet(obj, Variant(offset));
}

inline Variant& collectionOffsetGet(ObjectData* obj, litstr offset) {
  return collectionOffsetGet(obj, Variant(offset));
}

inline void collectionOffsetSet(ObjectData* obj, bool offset, CVarRef val) {
  collectionOffsetSet(obj, Variant(offset), val);
}

inline void collectionOffsetSet(ObjectData* obj, double offset, CVarRef val) {
  collectionOffsetSet(obj, Variant(offset), val);
}

inline void collectionOffsetSet(ObjectData* obj, litstr offset, CVarRef val) {
  collectionOffsetSet(obj, Variant(offset), val);
}

inline bool isOptimizableCollectionClass(const Class* klass) {
  return klass == c_Vector::classof() || klass == c_Map::classof() ||
         klass == c_StableMap::classof() || klass == c_Pair::classof();
}

void collectionSerialize(ObjectData* obj, VariableSerializer* serializer);

void throwOOB(int64_t key) ATTRIBUTE_NORETURN;

///////////////////////////////////////////////////////////////////////////////

}

#endif // incl_HPHP_EXT_COLLECTION_H_
