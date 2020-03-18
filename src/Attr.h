// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include "Obj.h"
#include "BroList.h"
#include "IntrusivePtr.h"

class Expr;

// Note that there are two kinds of attributes: the kind (here) which
// modify expressions or supply metadata on types, and the kind that
// are extra metadata on every variable instance.

typedef enum {
	ATTR_OPTIONAL,
	ATTR_DEFAULT,
	ATTR_REDEF,
	ATTR_ADD_FUNC,
	ATTR_DEL_FUNC,
	ATTR_EXPIRE_FUNC,
	ATTR_EXPIRE_READ,
	ATTR_EXPIRE_WRITE,
	ATTR_EXPIRE_CREATE,
	ATTR_RAW_OUTPUT,
	ATTR_PRIORITY,
	ATTR_GROUP,
	ATTR_LOG,
	ATTR_ERROR_HANDLER,
	ATTR_TYPE_COLUMN,	// for input framework
	ATTR_TRACKED,	// hidden attribute, tracked by NotifierRegistry
	ATTR_ON_CHANGE, // for table change tracking
	ATTR_DEPRECATED,
#define NUM_ATTRS (int(ATTR_DEPRECATED) + 1)
} attr_tag;

class Attr : public BroObj {
public:
	Attr(attr_tag t, IntrusivePtr<Expr> e);
	explicit Attr(attr_tag t);
	~Attr() override;

	attr_tag Tag() const	{ return tag; }
	Expr* AttrExpr() const	{ return expr.get(); }

	template<typename E>
	void SetAttrExpr(E&& e) { expr = std::forward<E>(e); }

	void Describe(ODesc* d) const override;
	void DescribeReST(ODesc* d, bool shorten = false) const;

	bool operator==(const Attr& other) const
		{
		if ( tag != other.tag )
			return false;

		if ( expr || other.expr )
			// If any has an expression and they aren't the same object, we
			// declare them unequal, as we can't really find out if the two
			// expressions are equivalent.
			return (expr == other.expr);

		return true;
		}

protected:
	void AddTag(ODesc* d) const;

	attr_tag tag;
	IntrusivePtr<Expr> expr;
};

// Manages a collection of attributes.
class Attributes : public BroObj {
public:
	Attributes(attr_list* a, IntrusivePtr<BroType> t, bool in_record, bool is_global);
	~Attributes() override;

	void AddAttr(IntrusivePtr<Attr> a);
	void AddAttrs(Attributes* a);	// Unref's 'a' when done

	Attr* FindAttr(attr_tag t) const;

	void RemoveAttr(attr_tag t);

	void Describe(ODesc* d) const override;
	void DescribeReST(ODesc* d, bool shorten = false) const;

	attr_list* Attrs()	{ return attrs; }

	bool operator==(const Attributes& other) const;

protected:
	void CheckAttr(Attr* attr);

	IntrusivePtr<BroType> type;
	attr_list* attrs;
	bool in_record;
	bool global_var;
};
