#include "LooseQuadtree.h"

#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <vector>

using namespace loose_quadtree;



#define ASSERT(CONDITION) if (!(CONDITION)) {\
	printf("Assertion failure %s:%d ASSERT(%s)\n", __FILE__, __LINE__, #CONDITION);\
	abort();\
	}


template <typename NumberT>
class TrivialBBExtractor {
public:
	static void ExtractBoundingBox(const BoundingBox<NumberT> *object, BoundingBox<NumberT> *bbox) {
		bbox->left = object->left;
		bbox->top = object->top;
		bbox->width = object->width;
		bbox->height = object->height;
	}
};



template <typename NumberT>
void TestBoundingBox() {
	BoundingBox<NumberT> big(100, 100, 200, 50);
	BoundingBox<NumberT> small_inside(200, 125, 5, 5);
	BoundingBox<NumberT> edge_inside(110, 110, 190, 40);
	BoundingBox<NumberT> edge_outside(300, 150, 20, 5);
	BoundingBox<NumberT> intersecting1(290, 90, 29, 25);
	BoundingBox<NumberT> intersecting2(290, 110, 29, 25);
	BoundingBox<NumberT> outside(290, 210, 29, 25);

	ASSERT(big.Contains(100, 100));
	ASSERT(!big.Contains(300, 150));

	ASSERT(big.Contains(big));
	ASSERT(big.Contains(small_inside));
	ASSERT(!small_inside.Contains(big));
	ASSERT(big.Contains(edge_inside));
	ASSERT(!edge_inside.Contains(big));
	ASSERT(!big.Contains(edge_outside));
	ASSERT(!edge_outside.Contains(big));
	ASSERT(!big.Contains(intersecting1));
	ASSERT(!intersecting1.Contains(big));
	ASSERT(!big.Contains(intersecting2));
	ASSERT(!intersecting1.Contains(big));
	ASSERT(!intersecting1.Contains(intersecting2));
	ASSERT(!big.Contains(outside));
	ASSERT(!outside.Contains(big));

	ASSERT(big.Intersects(big));
	ASSERT(big.Intersects(small_inside));
	ASSERT(small_inside.Intersects(big));
	ASSERT(big.Intersects(edge_inside));
	ASSERT(edge_inside.Intersects(big));
	ASSERT(!big.Intersects(edge_outside));
	ASSERT(!edge_outside.Intersects(big));
	ASSERT(big.Intersects(intersecting1));
	ASSERT(intersecting1.Intersects(big));
	ASSERT(big.Intersects(intersecting2));
	ASSERT(intersecting2.Intersects(big));
	ASSERT(intersecting1.Intersects(intersecting2));
	ASSERT(!big.Intersects(outside));
	ASSERT(!outside.Intersects(big));
}



template <typename NumberT>
void TestForwardTreeTraversal() {
	detail::BlocksAllocator allocator;
	detail::ForwardTreeTraversal<NumberT, BoundingBox<NumberT>> fortt;
	detail::TreeNode<BoundingBox<NumberT>> root(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tl(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tr(allocator);
	detail::TreeNode<BoundingBox<NumberT>> br(allocator);
	detail::TreeNode<BoundingBox<NumberT>> bl(allocator);
	root.top_left = &tl;
	tl.top_right = &tr;
	tr.bottom_right = &br;
	root.bottom_left = &bl;
	fortt.StartAt(&root, BoundingBox<NumberT>(0, 0, 64, 64));
	ASSERT(fortt.GetDepth() == 0);
	ASSERT(fortt.GetNode() == &root);
	ASSERT(fortt.GetNodeBoundingBox().left == 0);
	ASSERT(fortt.GetNodeBoundingBox().top == 0);
	ASSERT(fortt.GetNodeBoundingBox().width == 64);
	ASSERT(fortt.GetNodeBoundingBox().height == 64);
	fortt.GoTopLeft();
	ASSERT(fortt.GetDepth() == 1);
	ASSERT(fortt.GetNode() == &tl);
	ASSERT(fortt.GetNodeBoundingBox().left == 0);
	ASSERT(fortt.GetNodeBoundingBox().top == 0);
	ASSERT(fortt.GetNodeBoundingBox().width == 32);
	ASSERT(fortt.GetNodeBoundingBox().height == 32);
	fortt.GoTopRight();
	ASSERT(fortt.GetDepth() == 2);
	ASSERT(fortt.GetNode() == &tr);
	ASSERT(fortt.GetNodeBoundingBox().left == 16);
	ASSERT(fortt.GetNodeBoundingBox().top == 0);
	ASSERT(fortt.GetNodeBoundingBox().width == 16);
	ASSERT(fortt.GetNodeBoundingBox().height == 16);
	fortt.GoBottomRight();
	ASSERT(fortt.GetDepth() == 3);
	ASSERT(fortt.GetNode() == &br);
	ASSERT(fortt.GetNodeBoundingBox().left == 24);
	ASSERT(fortt.GetNodeBoundingBox().top == 8);
	ASSERT(fortt.GetNodeBoundingBox().width == 8);
	ASSERT(fortt.GetNodeBoundingBox().height == 8);
	fortt.StartAt(&root, BoundingBox<NumberT>(0, 0, 64, 64));
	ASSERT(fortt.GetDepth() == 0);
	ASSERT(fortt.GetNode() == &root);
	ASSERT(fortt.GetNodeBoundingBox().left == 0);
	ASSERT(fortt.GetNodeBoundingBox().top == 0);
	ASSERT(fortt.GetNodeBoundingBox().width == 64);
	ASSERT(fortt.GetNodeBoundingBox().height == 64);
	fortt.GoBottomLeft();
	ASSERT(fortt.GetDepth() == 1);
	ASSERT(fortt.GetNode() == &bl);
	ASSERT(fortt.GetNodeBoundingBox().left == 0);
	ASSERT(fortt.GetNodeBoundingBox().top == 32);
	ASSERT(fortt.GetNodeBoundingBox().width == 32);
	ASSERT(fortt.GetNodeBoundingBox().height == 32);
}

template <typename NumberT>
void TestFullTreeTraversal() {
	detail::BlocksAllocator allocator;
	detail::FullTreeTraversal<NumberT, BoundingBox<NumberT>> fultt;
	detail::TreeNode<BoundingBox<NumberT>> root(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tl(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tr(allocator);
	detail::TreeNode<BoundingBox<NumberT>> br(allocator);
	detail::TreeNode<BoundingBox<NumberT>> bl(allocator);
	root.top_left = &tl;
	tl.top_right = &tr;
	tr.bottom_right = &br;
	br.bottom_left = &bl;
	fultt.StartAt(&root, BoundingBox<NumberT>(0, 0, 64, 64));
	ASSERT(fultt.GetDepth() == 0);
	ASSERT(fultt.GetNode() == &root);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kNone);
	fultt.GoTopLeft();
	ASSERT(fultt.GetDepth() == 1);
	ASSERT(fultt.GetNode() == &tl);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kNone);
	fultt.GoTopRight();
	ASSERT(fultt.GetDepth() == 2);
	ASSERT(fultt.GetNode() == &tr);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kNone);
	fultt.GoBottomRight();
	ASSERT(fultt.GetDepth() == 3);
	ASSERT(fultt.GetNode() == &br);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kNone);
	fultt.GoBottomLeft();
	ASSERT(fultt.GetDepth() == 4);
	ASSERT(fultt.GetNode() == &bl);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kNone);
	fultt.GoUp();
	ASSERT(fultt.GetDepth() == 3);
	ASSERT(fultt.GetNode() == &br);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kBottomLeft);
	fultt.GoUp();
	ASSERT(fultt.GetDepth() == 2);
	ASSERT(fultt.GetNode() == &tr);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kBottomRight);
	fultt.GoUp();
	ASSERT(fultt.GetDepth() == 1);
	ASSERT(fultt.GetNode() == &tl);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kTopRight);
	fultt.GoUp();
	ASSERT(fultt.GetDepth() == 0);
	ASSERT(fultt.GetNode() == &root);
	ASSERT(fultt.GetNodeCurrentChild() == detail::ChildPosition::kTopLeft);
}

template <typename NumberT>
void TestBoundingBoxDiscrepancy() {
	detail::BlocksAllocator allocator;
	detail::FullTreeTraversal<NumberT, BoundingBox<NumberT>> ftt;
	detail::TreeNode<BoundingBox<NumberT>> root(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tl(allocator);
	detail::TreeNode<BoundingBox<NumberT>> tr(allocator);
	detail::TreeNode<BoundingBox<NumberT>> br(allocator);
	root.top_left = &tl;
	root.top_right = &tr;
	root.bottom_right = &br;
	ftt.StartAt(&root, BoundingBox<NumberT>(10, 10, 17, 19));
	NumberT orig_width = ftt.GetNodeBoundingBox().width;
	NumberT orig_height = ftt.GetNodeBoundingBox().height;
	ftt.GoTopLeft();
	NumberT tl_width = ftt.GetNodeBoundingBox().width;
	NumberT tl_height = ftt.GetNodeBoundingBox().height;
	ftt.GoUp();
	ftt.GoTopRight();
	NumberT tr_width = ftt.GetNodeBoundingBox().width;
	NumberT tr_height = ftt.GetNodeBoundingBox().height;
	ftt.GoUp();
	ftt.GoBottomRight();
	NumberT br_width = ftt.GetNodeBoundingBox().width;
	NumberT br_height = ftt.GetNodeBoundingBox().height;
	ftt.GoUp();
	ASSERT(orig_width == tl_width + tr_width);
	ASSERT(orig_height == tl_height + br_height);
	ASSERT(tr_width == br_width);
	ASSERT(tl_height == tr_height);
}

template <typename NumberT>
void TestTraversals() {
	TestForwardTreeTraversal<NumberT>();
	TestFullTreeTraversal<NumberT>();
	TestBoundingBoxDiscrepancy<NumberT>();
}


template <typename NumberT>
void TestInsertRemove(bool reclaim_losses) {
	std::vector<BoundingBox<NumberT>> objects;
	objects.emplace_back(1000, 1300, 50, 30);
	objects.emplace_back(1060, 1300, 50, 30);
	objects.emplace_back(1060, 1300, 5, 3);
	LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt;
	if (reclaim_losses) lqt.ForceCleanup();
	ASSERT(lqt.GetSize() == 0);
	ASSERT(lqt.IsEmpty());
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(lqt.GetSize() == 0);
	lqt.Insert(&objects[0]);
	ASSERT(lqt.GetSize() == 1);
	ASSERT(!lqt.IsEmpty());
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(!lqt.Contains(&objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	lqt.Remove(&objects[0]);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(lqt.GetSize() == 0);
	ASSERT(lqt.IsEmpty());
	if (reclaim_losses) lqt.ForceCleanup();

	lqt.Insert(&objects[1]);
	ASSERT(lqt.GetSize() == 1);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	lqt.Insert(&objects[0]);
	lqt.Insert(&objects[0]);
	lqt.Insert(&objects[0]);
	ASSERT(lqt.GetSize() == 2);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(!lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Insert(&objects[2]);
	ASSERT(lqt.GetSize() == 3);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[2]));
	ASSERT(!objects[2].Contains(lqt.GetLooseBoundingBox()));
	lqt.Remove(&objects[1]);
	lqt.Remove(&objects[1]);
	lqt.Remove(&objects[1]);
	ASSERT(lqt.GetSize() == 2);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(!lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[0]);
	ASSERT(lqt.GetSize() == 1);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(!lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[0]);
	ASSERT(lqt.GetSize() == 1);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(!lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[2]);
	ASSERT(lqt.GetSize() == 0);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(!lqt.Contains(&objects[1]));
	ASSERT(!lqt.Contains(&objects[2]));
	if (reclaim_losses) lqt.ForceCleanup();
}

template <typename NumberT>
void TestUpdate(bool reclaim_losses) {
	std::vector<BoundingBox<NumberT>> objects;
	objects.emplace_back(1000, 1000, 50, 30);
	objects.emplace_back(1060, 1000, 50, 30);
	objects.emplace_back(1060, 1000, 5, 3);
	LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt;
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Insert(&objects[0]);
	lqt.Insert(&objects[1]);
	lqt.Insert(&objects[2]);
	if (reclaim_losses) lqt.ForceCleanup();
	ASSERT(lqt.GetSize() == 3);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[2]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[2].Contains(lqt.GetLooseBoundingBox()));
	objects[2].width = 50;
	objects[2].height = 30;
	lqt.Update(&objects[2]);
	objects[0].left = 1060;
	lqt.Update(&objects[0]);
	ASSERT(lqt.GetSize() == 3);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[2]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[2].Contains(lqt.GetLooseBoundingBox()));
	if (reclaim_losses) lqt.ForceCleanup();
	ASSERT(lqt.GetSize() == 3);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[2]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[2].Contains(lqt.GetLooseBoundingBox()));
	lqt.Remove(&objects[0]);
	ASSERT(lqt.GetSize() == 2);
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[1]);
	ASSERT(lqt.GetSize() == 1);
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[2]);
	ASSERT(lqt.GetSize() == 0);
	if (reclaim_losses) lqt.ForceCleanup();
}

template <typename NumberT>
void TestMoreTrees(bool reclaim_losses) {
	std::vector<BoundingBox<NumberT>> objects;
	objects.emplace_back(1000, 1000, 50, 30);
	objects.emplace_back(1060, 1000, 50, 30);
	objects.emplace_back(1060, 1000, 5, 3);
	LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt;
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Insert(&objects[0]);
	lqt.Insert(&objects[1]);
	ASSERT(lqt.GetSize() == 2);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	if (reclaim_losses) lqt.ForceCleanup();
	{
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt2;
		if (reclaim_losses) lqt2.ForceCleanup();
		ASSERT(lqt2.GetSize() == 0);
		lqt2.Insert(&objects[1]);
		lqt2.Insert(&objects[2]);
		lqt.Insert(&objects[2]);
		if (reclaim_losses) lqt.ForceCleanup();
		ASSERT(lqt2.GetSize() == 2);
		ASSERT(!lqt2.Contains(&objects[0]));
		ASSERT(lqt2.Contains(&objects[1]));
		ASSERT(lqt2.Contains(&objects[2]));
		if (reclaim_losses) lqt2.ForceCleanup();
		lqt2.Remove(&objects[1]);
		lqt2.Remove(&objects[2]);
		ASSERT(lqt2.GetSize() == 0);
		if (reclaim_losses) lqt2.ForceCleanup();
	}
	{
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt2;
		lqt2.Insert(&objects[1]);
		if (reclaim_losses) lqt2.ForceCleanup();
	}
	{
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt2;
		lqt2.Insert(&objects[1]);
		lqt2.Insert(&objects[2]);
		lqt2.Insert(&objects[0]);
		ASSERT(lqt2.Contains(&objects[1]));
		lqt2.Clear();
		ASSERT(lqt2.GetSize() == 0);
		ASSERT(!lqt2.Contains(&objects[1]));
		if (reclaim_losses) lqt2.ForceCleanup();
		ASSERT(lqt2.GetSize() == 0);
		ASSERT(!lqt2.Contains(&objects[1]));
		lqt2.Insert(&objects[1]);
		ASSERT(lqt2.GetSize() == 1);
		ASSERT(lqt2.Contains(&objects[1]));
	}
	ASSERT(lqt.GetSize() == 3);
	ASSERT(lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.Contains(&objects[2]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[0]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[1]));
	ASSERT(lqt.GetLooseBoundingBox().Intersects(objects[2]));
	ASSERT(!objects[0].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[1].Contains(lqt.GetLooseBoundingBox()));
	ASSERT(!objects[2].Contains(lqt.GetLooseBoundingBox()));
	lqt.Remove(&objects[0]);
	lqt.Remove(&objects[2]);
	ASSERT(!lqt.Contains(&objects[0]));
	ASSERT(lqt.Contains(&objects[1]));
	ASSERT(lqt.GetSize() == 1);
	if (reclaim_losses) lqt.ForceCleanup();
	lqt.Remove(&objects[1]);
	ASSERT(lqt.GetSize() == 0);
	if (reclaim_losses) lqt.ForceCleanup();
}

template <typename NumberT>
void TestContainer() {
	TestInsertRemove<NumberT>(false);
	TestInsertRemove<NumberT>(true);
	TestUpdate<NumberT>(false);
	TestUpdate<NumberT>(true);
	TestMoreTrees<NumberT>(false);
	TestMoreTrees<NumberT>(true);
}



template <typename NumberT>
void TestQueryIntersects(const std::vector<BoundingBox<NumberT>>& objects,
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>>& lqt) {
	auto query = lqt.QueryIntersectsRegion(BoundingBox<NumberT>(33,33,1,1));
	ASSERT(query.EndOfQuery());

	query = lqt.QueryIntersectsRegion(BoundingBox<NumberT>(9000,9000,9000,9000));
	int count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		(void)obj;
		count++;
		query.Next();
	}
	ASSERT(count == 7);

	query = lqt.QueryIntersectsRegion(BoundingBox<NumberT>(10003,10003,3,7));
	count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[0] && obj != &objects[1] && obj != &objects[2]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 3);

	query = lqt.QueryIntersectsRegion(BoundingBox<NumberT>(14900,14900,200,200));
	count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[0] && obj != &objects[1] && obj != &objects[3] && obj != &objects[5]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 4);
}

template <typename NumberT>
void TestQueryInside(const std::vector<BoundingBox<NumberT>>& objects,
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>>& lqt) {
	auto query = lqt.QueryInsideRegion(BoundingBox<NumberT>(33,33,1,1));
	ASSERT(query.EndOfQuery());

	query = lqt.QueryInsideRegion(BoundingBox<NumberT>(9000,9000,9000,9000));
	int count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		(void)obj;
		count++;
		query.Next();
	}
	ASSERT(count == 7);

	query = lqt.QueryInsideRegion(BoundingBox<NumberT>(10003,10003,3,7));
	ASSERT(query.EndOfQuery());

	query = lqt.QueryInsideRegion(BoundingBox<NumberT>(14900,14900,300,300));
	count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[5] && obj != &objects[6]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 2);
}

template <typename NumberT>
void TestQueryContains(const std::vector<BoundingBox<NumberT>>& objects,
		LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>>& lqt) {
	auto query = lqt.QueryContainsRegion(BoundingBox<NumberT>(33,33,1,1));
	ASSERT(query.EndOfQuery());

	query = lqt.QueryContainsRegion(BoundingBox<NumberT>(9000,9000,9000,9000));
	ASSERT(query.EndOfQuery());

	query = lqt.QueryContainsRegion(BoundingBox<NumberT>(10003,10003,3,7));
	int count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[0] && obj != &objects[1]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 2);

	query = lqt.QueryContainsRegion(BoundingBox<NumberT>(14900,14900,200,200));
	count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[0] && obj != &objects[1]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 2);

	query = lqt.QueryContainsRegion(BoundingBox<NumberT>(15000,15000,2,2));
	count = 0;
	while (!query.EndOfQuery()) {
		BoundingBox<NumberT>* obj = query.GetCurrent();
		if (obj != &objects[0] && obj != &objects[1] && obj != &objects[3] && obj != &objects[5]) {
			ASSERT(false);
		}
		count++;
		query.Next();
	}
	ASSERT(count == 4);
}

template <typename NumberT>
void TestQueries() {
	std::vector<BoundingBox<NumberT>> objects;
	objects.emplace_back(10000, 10000, 8000, 8000);//0
	objects.emplace_back(10000, 10000, 7000, 6000);//1
	objects.emplace_back(10000, 10000, 7, 6);//2
	objects.emplace_back(15000, 15000, 500, 600);//3
	objects.emplace_back(15100, 15100, 200, 200);//4
	objects.emplace_back(15000, 15000, 200, 200);//5
	objects.emplace_back(15100, 15100, 2, 2);//6
	LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt;
	for (auto& obj : objects) {
		lqt.Insert(&obj);
	}
	TestQueryIntersects<NumberT>(objects, lqt);
	TestQueryInside<NumberT>(objects, lqt);
	TestQueryContains<NumberT>(objects, lqt);
}



template <typename NumberT>
void StressTest() {
#ifndef NDEBUG
	const int objects_generated = 10000;
	const int object_fluctuation = 1000;
#else
	const int objects_generated = 200000;
	const int object_fluctuation = 20000;
#endif
	const int full_rounds = 24;
	const int query_rounds = 4;
	std::minstd_rand rand;
	std::uniform_int_distribution<std::size_t> index(0, objects_generated - 1);
	std::uniform_real_distribution<float>
		coordinate(std::is_integral<NumberT>::value ?
					(float)(std::is_signed<NumberT>::value ?
					-std::numeric_limits<NumberT>::max() / 8 :
					std::numeric_limits<NumberT>::max() / 16 * 7) :
					-1.0f,
				std::is_integral<NumberT>::value ?
					(float)(std::is_signed<NumberT>::value ?
					std::numeric_limits<NumberT>::max() / 8 :
					std::numeric_limits<NumberT>::max() / 16 * 9) :
					1.0f);
	std::uniform_real_distribution<float>
		distance(0.0f, std::is_integral<NumberT>::value ?
					(float)(std::is_signed<NumberT>::value ?
					std::numeric_limits<NumberT>::max() / 8 :
					std::numeric_limits<NumberT>::max() / 16) :
					0.5f);

	std::vector<BoundingBox<NumberT>> objects;
	objects.reserve(objects_generated);
	std::vector<bool> flags(objects_generated, false);
	LooseQuadtree<NumberT, BoundingBox<NumberT>, TrivialBBExtractor<NumberT>> lqt;
	for (std::size_t i = 0; i < objects_generated; i++) {
		objects.emplace_back((NumberT)coordinate(rand), (NumberT)coordinate(rand),
				(NumberT)distance(rand), (NumberT)distance(rand));
		lqt.Insert(&objects[i]);
	}
	ASSERT(objects.size() == objects_generated);
	ASSERT(flags.size() == objects_generated);

	for (int round = 0; round < full_rounds; round++) {
		for (int fluctobj = 0; fluctobj < object_fluctuation; fluctobj++) {
			std::size_t id = index(rand);
			objects[id] = BoundingBox<NumberT>((NumberT)coordinate(rand), (NumberT)coordinate(rand),
					(NumberT)distance(rand), (NumberT)distance(rand));
			lqt.Update(&objects[id]);
		}
		for (int query_round = 0; query_round < query_rounds; query_round++) {
			BoundingBox<NumberT> query_region((NumberT)coordinate(rand), (NumberT)coordinate(rand),
					(NumberT)distance(rand), (NumberT)distance(rand));
			for (std::size_t i = 0; i < objects_generated; i++) {
				flags[i] = false;
			}

			auto query = lqt.QueryIntersectsRegion(query_region);
			while (!query.EndOfQuery()) {
				BoundingBox<NumberT>* obj = query.GetCurrent();
				ASSERT(query_region.Intersects(*obj));
				std::size_t id = (std::size_t)(obj - &objects[0]);
				ASSERT(id >= 0 && id < objects_generated);
				flags[id] = true;
				query.Next();
			}
			for (std::size_t i = 0; i < objects_generated; i++) {
				ASSERT(flags[i] == query_region.Intersects(objects[i]));
				flags[i] = false;
			}

			query = lqt.QueryInsideRegion(query_region);
			while (!query.EndOfQuery()) {
				BoundingBox<NumberT>* obj = query.GetCurrent();
				ASSERT(query_region.Contains(*obj));
				std::size_t id = (std::size_t)(obj - &objects[0]);
				ASSERT(id >= 0 && id < objects_generated);
				flags[id] = true;
				query.Next();
			}
			for (std::size_t i = 0; i < objects_generated; i++) {
				ASSERT(flags[i] == query_region.Contains(objects[i]));
				flags[i] = false;
			}

			query = lqt.QueryContainsRegion(query_region);
			while (!query.EndOfQuery()) {
				BoundingBox<NumberT>* obj = query.GetCurrent();
				ASSERT(obj->Contains(query_region));
				std::size_t id = (std::size_t)(obj - &objects[0]);
				ASSERT(id >= 0 && id < objects_generated);
				flags[id] = true;
				query.Next();
			}
			for (std::size_t i = 0; i < objects_generated; i++) {
				ASSERT(flags[i] == objects[i].Contains(query_region));
				//flags[i] = false;
			}
		}
	}
	lqt.ForceCleanup();
}



template <typename NumberT>
void RunTests(const char* type_str) {
	printf("***** Running tests for %s (%lu-bit)... ", type_str, sizeof(NumberT) * 8);
	auto start = std::chrono::high_resolution_clock::now();
	TestBoundingBox<NumberT>();
	TestTraversals<NumberT>();
	TestContainer<NumberT>();
	TestQueries<NumberT>();
	StressTest<NumberT>();
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time = end - start;
	printf("took %f seconds\n", time.count());
}



int main(int, char*[]) {
	puts("***** Testing is about to start *****");
	printf("***** This system is %lu-bit\n", sizeof(void*) * 8);
	RunTests<float>("float");
	RunTests<double>("double");
	RunTests<long double>("long double");
	RunTests<int>("int");
	RunTests<long>("long");
	RunTests<short>("short");
	RunTests<unsigned int>("unsigned int");
	RunTests<unsigned long>("unsigned long");
	RunTests<unsigned short>("unsigned short");
	RunTests<long long>("long long");
	puts("***** Testing is successfully finished *****");
	return 0;
}

