#pragma once
#include <windows.h>
#include <xnamath.h>
#include <memory>
#include <vector>
#include <stdlib.h>
#include "Lemmi2DAABB.h"

template<class T>
class QuadTreeNode
{
	private:
	static const int maxDepth = 6; //Depth defines how many times the original node splits. Every node splits into 4 new nodes. -> 4^maxItems.
	static const int maxItems = 10; //How many items a node can contain before it splits.

	float size;
	int depth; 
	XMFLOAT2 centerPosition;

	Lemmi2DAABB* boundingBox;
	std::vector<QuadTreeNode> children;
	std::vector<std::shared_ptr<T>> content;
	
	protected: 
	std::shared_ptr<QuadTreeNode> parent;

	public:

	QuadTreeNode(std::shared_ptr<QuadTreeNode> parent, Lemmi2DAABB box, int depth)
	{
		this.parent = parent;
		this->depth = depth;

		boundingBox = new Lemmi2DAABB(&box);

		XMStoreFloat(&size, XMVector2Length(XMLoadFloat2(box.MaxPoint - box.MinPoint)));

		XMStoreFloat2(centerPosition, XMLoadFloat2(&box.MinPoint) + 
			(0.5f * size * XMVector2Normalize(XMLoadFloat2(box.MaxPoint - box.MinPoint))));
	}

	QuadTreeNode(std::shared_ptr<QuadTreeNode> parent, XMFLOAT2 center, float size, int depth)
	{
		this.parent = parent;
		this->size = size;
		this->centerPosition = center;
		this->depth = depth;

		XMFLOAT2 minPoint, maxPoint;
		XMStoreFloat2(&minPoint, (XMLoadFloat2(&center) - (XMVectorSplatOne() * size * 0.5f)))
		XMStoreFloat2(&maxPoint, (XMLoadFloat2(&center) + (XMVectorSplatOne() * size * 0.5f)))

		boundingBox = new Lemmi2DAABB(minPoint, maxPoint);
	}

	~QuadTreeNode()
	{
		delete boundingBox;
	}

	protected:

	/// <summary>
	/// Divide node and create sub-nodes
	/// </summary>
	void CreateSubNodes()
	{
		if(children.size() == 0)
		{
			for(int i = -1; i < 2; i += 2)
			{
				for(int j = -1; j < 2; j += 2)
				{
						XMFLOAT2 subCenter = new XMFLOAT2(Center.X + (0.25f * size * (float)i), Center.Y + (0.25f * size * (float)j));
						float subSize = size * 0.5f;

						QuadTreeNode newNode = QuadTreeNode(this, subCenter, subSize, depth + 1);

						children.push_back(newNode);
				}
			}
		}
	}

	///// <summary>
	///// Propagates objects down the tree up to maxDepth
	///// </summary>
	//public void Propagate()
	//{
	//	if(Content.Count > maxItems && Depth <= maxDepth)
	//	{
	//		CreateSubNodes();

	//		for(int index = Content.Count - 1; index >= 0; index--)
	//		{
	//			switch(OctreeContainmentMode)
	//			{
	//			case OctreeContainmentModes.Intersects:
	//				T item = Content[index];
	//				Content.Remove(Content[index]);

	//				for(int subNodeIndex = 0; subNodeIndex < SubNodes.Count; subNodeIndex++)
	//				{
	//					if(item.Intersect(SubNodes[subNodeIndex].BoundingBox))
	//					{
	//						SubNodes[subNodeIndex].Content.Add(item);
	//					}
	//				}
	//				break;

	//			case OctreeContainmentModes.Contains:
	//				for(int subNodeIndex = 0; subNodeIndex < SubNodes.Count; subNodeIndex++)
	//				{
	//					if(Content[index].IsContained(SubNodes[subNodeIndex].BoundingBox) == ContainmentType.Contains)
	//					{
	//						SubNodes[subNodeIndex].Content.Add(Content[index]);
	//						Content.Remove(Content[index]);
	//						break;
	//					}
	//				}
	//				break;
	//			}
	//		}

	//		//Propagate subnodes
	//		for(int subNodeIndex = 0; subNodeIndex < SubNodes.Count; subNodeIndex++)
	//		{
	//			SubNodes[subNodeIndex].Propagate();

	//		}
	//	}
	//}

};

/*
/// <summary>
/// Node of the Octree
/// </summary>
/// <typeparam name="T">Object of Type IOctreeable</typeparam>
public class OctreeNode<T> where T :IOctreeable
{






/// <summary>
/// Clean up all empty subnodes and report if current node is empty
/// </summary>
/// <returns>True is node is empty</returns>
public bool CleanUpSubNodes()
{
	for(int subNodeIndex = SubNodes.Count - 1; subNodeIndex >= 0; subNodeIndex--)
	{
		if(SubNodes[subNodeIndex].CleanUpSubNodes())
		{
			SubNodes.Remove(SubNodes[subNodeIndex]);
		}
	}

if(Content.Count > 0)
return false;

if(SubNodes.Count > 0)
return false;

//No content and all subnodes have been removed
return true;
}

/// <summary>
/// Get content of node if it intersects
/// </summary>
/// <param name="test">Test to perform</param>
/// <param name="results">List to add content too</param>
/// <param name="queryID">Current query cycle</param>
public void Get(Predicate<BoundingBox> test, List<T> results, int queryID)
{
if(test.Invoke(BoundingBox)) //(sphere.Contains(BoundingBox) != ContainmentType.Disjoint)
{
//process children
for(int subNodeIndex = 0; subNodeIndex < SubNodes.Count; subNodeIndex++)
{
SubNodes[subNodeIndex].Get(test, results, queryID);
}

//Add content
for(int contentIndex = 0; contentIndex < Content.Count; contentIndex++)
{
if(!Content[contentIndex].Destroyed) // !Content[contentIndex].Destroyed   <- Was added to make sure we don't fetch destroyed objects. Can be removed if you don't think you need it.
{
if(Content[contentIndex].LastQueryID != queryID) //I split the if checks up because only checking a bool instead of checking a bool + integer value is faster.
{
Content[contentIndex].LastQueryID = queryID;
results.Add(Content[contentIndex]);
}
}
}
}
}
}
*/




/*
	public enum OctreeContainmentModes
	{
		/// <summary>
		/// Nodes must fully contain an object to own it
		/// </summary>
		Contains,
		/// <summary>
		/// Nodes only need to intersect with an object to have a reference to it. LastQueryID is used to prune results when searching nodes
		/// </summary>
		Intersects
	}

	/// <summary>
	/// Octree class
	/// </summary>
	/// <typeparam name="T">Type of object held in Octree</typeparam>
	public class Octree<T> where T : IOctreeable
	{
		private OctreeNode<T> root = null;
		private int queryID = 0;
		private List<T> results = new List<T>();

		public Octree(float size, List<T> items)
		{
			root = new OctreeNode<T>(null, Vector3.Zero, size, 0);
			root.Content.AddRange(items);
			root.Propagate();
		}

		public Octree(BoundingBox box, List<T> items, OctreeContainmentModes mode)
		{
			float halfsize = (box.Max - box.Min).Length() * 0.5f;
			Vector3 center = (box.Min + (halfsize * Vector3.Normalize(box.Max - box.Min)) ) ;
			root = new OctreeNode<T>(null, center, halfsize * 2, 0);
			root.OctreeContainmentMode = mode;
			root.Content.AddRange(items);
			root.Propagate();
			root.CleanUpSubNodes();
		}

		public void Update()
		{
			root.Propagate();
			root.CleanUpSubNodes();
		}

		/// <summary>
		/// Get a list of objects from nodes intersecting a sphere
		/// </summary>
		/// <param name="sphere">Sphere to test against the Octree</param>
		/// <returns>List of objects</returns>
		public List<T> Get(BoundingSphere sphere)
		{
			results.Clear();
			queryID++;
			root.Get(node => sphere.Contains(node) != ContainmentType.Disjoint, results, queryID);

			return results;
		}


		/// <summary>
		/// Get a list of objects from nodes intersecting a box
		/// </summary>
		/// <param name="box">Box to test against the Octree</param>
		/// <returns>List of objects</returns>
		public List<T> Get(BoundingBox box)
		{
			results.Clear();
			queryID++;
			root.Get(node => box.Contains(node) != ContainmentType.Disjoint, results, queryID);

			return results;
		}

		/// <summary>
		/// Get a list of objects from nodes intersecting a frustum
		/// </summary>
		/// <param name="box">Frustum to test against the Octree</param>
		/// <returns>List of objects</returns>
		public List<T> Get(BoundingFrustum frustum)
		{
			results.Clear();
			queryID++;
			root.Get(node => frustum.Contains(node) != ContainmentType.Disjoint, results, queryID);

			return results;
		}

		/// <summary>
		/// Get a list of objects from nodes intersecting a ray
		/// </summary>
		/// <param name="ray">Ray</param>
		/// <param name="range">Ray's max range</param>
		/// <returns>List of objects</returns>
		public List<T> Get(Ray ray, float range)
		{
			results.Clear();
			queryID++;
			root.Get(node =>
			{
				float? d = ray.Intersects(node);
				return (d != null && d <= range);
			}, results, queryID);

			return results;
		}
	}
*/

















/************************************************************************/
/* IOctreeable implementations                                          */
/************************************************************************/

///// <summary>
///// Interface to define objects that can be tested againt bounding primitives
///// </summary>
//public interface IOctreeable
//{
//	ContainmentType IsContained(BoundingBox box);
//	ContainmentType IsContained(BoundingSphere sphere);
//	ContainmentType IsContained(BoundingFrustum frustum);
//
//	bool Intersect(BoundingBox box);
//	bool Intersect(BoundingSphere sphere);
//	bool Intersect(BoundingFrustum frustum);
//
//	int LastQueryID
//	{
//		get;
//		set;
//	}
//
//	BoundingBox BoundingBox
//	{
//		get;
//		set;
//	}
//
//	bool Destroyed
//	{
//		get;
//	}
//}
//}

//#region Octree members
//private int m_lastQueryID = -1;
//private BoundingBox m_boundingBox;
//
//public ContainmentType IsContained(BoundingBox box)
//{
//	ContainmentType contains = ContainmentType.Disjoint;
//
//	if(m_boundingBox.Contains(box) == ContainmentType.Contains)
//	{
//		contains = ContainmentType.Contains;
//	}
//	return contains;
//}
//
//public ContainmentType IsContained(BoundingSphere sphere)
//{
//	ContainmentType contains = ContainmentType.Disjoint;
//
//	if(m_boundingBox.Contains(sphere) == ContainmentType.Contains)
//	{
//		contains = ContainmentType.Contains;
//	}
//	return contains;
//}
//
//public ContainmentType IsContained(BoundingFrustum frustum)
//{
//	ContainmentType contains = ContainmentType.Disjoint;
//
//
//	if(m_boundingBox.Contains(frustum) == ContainmentType.Contains)
//	{
//		contains = ContainmentType.Contains;
//	}
//	return contains;
//}
//
//public bool Intersect(BoundingBox box)
//{
//	if(IsContained(box) != ContainmentType.Disjoint)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//public bool Intersect(BoundingSphere sphere)
//{
//	if(IsContained(sphere) != ContainmentType.Disjoint)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//public bool Intersect(BoundingFrustum frustum)
//{
//	if(IsContained(frustum) != ContainmentType.Disjoint)
//	{
//		return true;
//	}
//	return false;
//}
//
//public int LastQueryID
//{
//	get
//	{
//		return m_lastQueryID;
//	}
//	set
//	{
//		m_lastQueryID = value;
//	}
//}
//
//public BoundingBox BoundingBox
//{
//	get
//	{
//		return m_boundingBox;
//	}
//	set
//	{
//		m_boundingBox = value;
//	}
//}
//#endregion
