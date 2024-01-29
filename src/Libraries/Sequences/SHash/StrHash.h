/******************************************************************************************************************************/ /*
																																  * @file StrHash.h
																																  *
																																  * Declaration of StrHash class. This class implements a hash table having as key value a string.
																																  *
																																  * @author Thanasis Vergoulis
																																  ********************************************************************************************************************************/
#ifndef STRHASH_H
#define STRHASH_H

#include <iostream> //for the use of cout
#include <vector>
#include <tr1/unordered_map> //for the use of hash function
#include <boost/dynamic_bitset.hpp>
using namespace std; // to recognize keywords like std::string, std::cout, etc without writing std
using namespace boost;

/**
 * @brief A hash table having as key value a string.
 *
 * @var _hash_func The hash function we use to map sequences to (hash) array positions.
 * @var _buckets_num The number of buckets in the hash table.
 * @var _key_len The length of the strings used as keys.
 * @var _buckets Array containing pointers to the hash buckets.
 *
 * @author Thanasis Vergoulis
 */
template <class HBucket>
class StrHash
{
protected:
	tr1::unordered_map<string, int>::hasher _hash_func;
	unsigned long _buckets_num;
	unsigned long _buck_groups_num;
	unsigned long _buckets_per_group;
	unsigned long _key_len;
	HBucket ***_buckets;
	dynamic_bitset<> *_group_status;

public:
	/**
	 * The constructor of the class.
	 *
	 * It allocates space and does initializations for some class members.
	 *
	 * @attention As StrHash is a template-class, the implementations of all the member-methods are included inside the header
	 * file as inline implementations (for details, see also in the following link:
	 * http://www.parashift.com/c%2B%2B-faq-lite/templates-defn-vs-decl.html).
	 *
	 * @param key_len The length of the key-strings.
	 * @param buckets_num The number of buckets in the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	StrHash(unsigned long key_len, unsigned long buckets_num, unsigned long buck_groups_num)
	{
		this->_key_len = key_len;				  // Get the length of the key-strings.
		this->_buckets_num = buckets_num;		  // Get the number of buckets.
		this->_buck_groups_num = buck_groups_num; // Get the number of bucket groups.

		if ((this->_buckets_num % this->_buck_groups_num) != 0)
			cout << "WARNING!! the number of groups does not divide exactly the number of buckets" << endl;

		this->_buckets_per_group = this->_buckets_num / this->_buck_groups_num;

		this->_buckets = new HBucket **[this->_buck_groups_num]; // Allocate space for the group of buckets.
		for (unsigned long i = 0; i < this->_buck_groups_num; i++)
			this->_buckets[i] = 0;

		this->_group_status = new dynamic_bitset<>(this->_buck_groups_num);
	}

	StrHash(void)
	{
	}

	~StrHash(void)
	{
		delete this->_group_status;
	}

	void setGroupStatus(unsigned long bucket_id)
	{
		unsigned long group_id;
		group_id = bucket_id / this->_buckets_per_group;
		// cout << "group_id=" << group_id << endl;
		if (!this->_group_status->test(group_id))
		{
			this->_group_status->set(group_id);
			this->_buckets[group_id] = new HBucket *[this->_buckets_per_group];
			// cout << "after" << endl;
			for (unsigned long j = 0; j < this->_buckets_per_group; j++)
			{
				// cout << "j=" << j << endl;
				this->_buckets[group_id][j] = 0;
			}
		}
		// cout << "test" << endl;
	}

	bool emptyGroup(unsigned long group_id)
	{
		if (this->_group_status->test(group_id))
			return false;
		else
			return true;
	}

	void deleteGroup(unsigned long group_id)
	{											 // cout<<"delete group "<<group_id<<endl; cout<<"groups to delete: "<<this->_group_status->size()<<endl;
		if (this->_group_status->test(group_id)) // if the group is not empty
		{										 // cout<<group_id<<endl;//cout<<"deleting group "<<group_id<<endl;
			for (unsigned long i = 0; i < this->_buckets_per_group; i++)
			{
				if (this->_buckets[group_id][i] != 0)
				{
					delete this->_buckets[group_id][i];
				}
			}
			delete[] this->_buckets[group_id];
		}
		/*		else
				{
					cout<<"EMPTY GROUP = "<<group_id<<endl;
				}*/
	}

	//	virtual void deleteGroupBucket(unsigned long group_id,unsigned long bucket_id)=0;

	/**
	 * It returns the ID of the bucket into which key is hashed.
	 *
	 * @param key The key-string.
	 *
	 * @return The ID of the bucket (If there is no bucket, the id of the "empty" bucket is returned.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long find(string key)
	{
		unsigned long hash_value = this->_hash_func(key) % this->_buckets_num;
		HBucket *tmp;
		tmp = this->getBucketPtr(hash_value);
		//		if( this->_buckets[hash_value]!=0)
		if (tmp != 0)
		{
			return (hash_value);
		}
		else
		{
			return this->_buckets_num + 1;
		}
	};

	/**
	 * It returns the number of buckets.
	 *
	 * @return The number of buckets.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long size()
	{
		return this->_buckets_num;
	};

	/**
	 * It returns the number of occupied buckets.
	 *
	 * @return The number of buckets containing pointer to one HBucket.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long occBuckets()
	{
		unsigned long occ_buckets_num = 0;
		HBucket *tmp;
		for (unsigned long i = 0; i < this->_buckets_num; i++)
		{
			tmp = this->getBucketPtr(i);
			//			if( this->_buckets[i]!=0 )
			if (tmp != 0)
			{
				occ_buckets_num++;
			}
		}
		return occ_buckets_num;
	};

	/**
	 * It returns the size of the hash table in bits.
	 *
	 * @note It is an abstract method. It MUST be implemented by derived classes (compiler problem otherwise).
	 *
	 * @return The size of the hash table in bits.
	 *
	 * @author Thanasis Vergoulis
	 */
	//	virtual double bSize(void)=0;

	/**
	 * It prints on the standard output statistics which are special for the derived class.
	 *
	 * @note It is an abstract method. It MUST be implemented by derived classes (compiler problem otherwise).
	 *
	 * @author Thanasis Vergoulis
	 */
	virtual void extraStatistics() = 0;

	/**
	 * It prints on the standard output some statistics for the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void statistics()
	{
		cout << "[HASH INDEX STATISTICS:]" << endl;
		cout << "buckets num: " << this->size() << "(occupied: " << this->occBuckets() << ")" << endl;
		//		cout<<"size in bits: "<<this->bSize()<<endl;
		this->extraStatistics();
		cout << endl;
	};

	/**
	 * It prints on the standard output the contents of the hash table.
	 *
	 * @attention Don't try this if the hash table is really large. This is useful only for debugging reasons.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print()
	{
		HBucket *tmp;
		for (unsigned long i = 0; i < this->size(); i++)
		{
			tmp = this->getBucketPtr(i);
			if (tmp != 0)
			//			if( this->_buckets[i]!=0 )
			{
				//				cout<<(*this->_buckets[i]).print(); cout<<endl;
				cout << tmp->print();
				cout << endl;
			}
		}
	}

	HBucket *getBucketPtr(unsigned long idx)
	{
		unsigned long group_id;
		unsigned long bucket_id;

		group_id = idx / this->_buckets_per_group;
		bucket_id = idx % this->_buckets_per_group;

		if (this->_buckets[group_id] == 0)
			return 0;
		else
			return this->_buckets[group_id][bucket_id];
	}

	HBucket **getBucketPtrRef(unsigned long idx)
	{
		unsigned long group_id;
		unsigned long bucket_id;

		group_id = idx / this->_buckets_per_group;
		bucket_id = idx % this->_buckets_per_group;

		return &this->_buckets[group_id][bucket_id];
	}

	/*	void setBucketPtr(unsigned long idx, HBucket* ptr)
		{
			unsigned long group_id;
			unsigned long bucket_id;

			group_id = idx/this->_buckets_per_group;
			bucket_id = idx%this->_buckets_per_group;

			this->_buckets[group_id][bucket_id] = ptr;
		}*/
};

template <class HBucket>
class StrHashSimple
{
protected:
	tr1::unordered_map<string, int>::hasher _hash_func;
	unsigned long _buckets_num;
	unsigned long _key_len;
	HBucket **_buckets;

public:
	/**
	 * The constructor of the class.
	 *
	 * It allocates space and does initializations for some class members.
	 *
	 * @attention As StrHash is a template-class, the implementations of all the member-methods are included inside the header
	 * file as inline implementations (for details, see also in the following link:
	 * http://www.parashift.com/c%2B%2B-faq-lite/templates-defn-vs-decl.html).
	 *
	 * @param key_len The length of the key-strings.
	 * @param buckets_num The number of buckets in the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	StrHashSimple(unsigned long key_len, unsigned long buckets_num)
	{
		this->_key_len = key_len;		  // Get the length of the key-strings.
		this->_buckets_num = buckets_num; // Get the number of buckets.

		this->_buckets = new HBucket **[this->_buckets_num]; // Allocate space for the buckets.
		for (int i = 0; i < this->_buckets_num; i++)
			this->_buckets[i] = 0;
	}

	StrHashSimple(void)
	{
	}

	~StrHashSimple(void)
	{
	}

	/**
	 * It returns the ID of the bucket into which key is hashed.
	 *
	 * @param key The key-string.
	 *
	 * @return The ID of the bucket (If there is no bucket, the id of the "empty" bucket is returned.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long find(string key)
	{
		unsigned long hash_value = this->_hash_func(key) % this->_buckets_num;

		if (this->_buckets[hash_value] != 0)
		{
			return (hash_value);
		}
		else
		{
			return this->_buckets_num + 1;
		}
	};

	/**
	 * It returns the number of buckets.
	 *
	 * @return The number of buckets.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long size()
	{
		return this->_buckets_num;
	};

	/**
	 * It returns the number of occupied buckets.
	 *
	 * @return The number of buckets containing pointer to one HBucket.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long occBuckets()
	{
		unsigned long occ_buckets_num = 0;

		for (unsigned long i = 0; i < this->_buckets_num; i++)
		{
			if (this->_buckets[i] != 0)
			{
				occ_buckets_num++;
			}
		}
		return occ_buckets_num;
	};

	/**
	 * It prints on the standard output statistics which are special for the derived class.
	 *
	 * @note It is an abstract method. It MUST be implemented by derived classes (compiler problem otherwise).
	 *
	 * @author Thanasis Vergoulis
	 */
	virtual void extraStatistics() = 0;

	/**
	 * It prints on the standard output some statistics for the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void statistics()
	{
		cout << "[HASH INDEX STATISTICS:]" << endl;
		cout << "buckets num: " << this->size() << "(occupied: " << this->occBuckets() << ")" << endl;
		this->extraStatistics();
		cout << endl;
	};

	/**
	 * It prints on the standard output the contents of the hash table.
	 *
	 * @attention Don't try this if the hash table is really large. This is useful only for debugging reasons.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print()
	{
		for (unsigned long i = 0; i < this->size(); i++)
		{
			if (this->_buckets[i] != 0)
			{
				cout << (*this->_buckets[i]).print();
				cout << endl;
			}
		}
	}
};

#endif
