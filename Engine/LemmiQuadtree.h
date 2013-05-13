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

	QuadTreeNode(QuadTreeNode* parent, Lemmi2DAABB box, int depth)
	{
		this->parent = make_shared(parent);
		this->depth = depth;

		boundingBox = box;

		XMStoreFloat(&size, XMVector2Length(XMLoadFloat2(&box.MaxPoint) - XMLoadFloat2(&box.MinPoint)));

		XMStoreFloat2(&centerPosition, 
		XMLoadFloat2(&box.MinPoint) + (0.5f * size * XMVector2Normalize(XMLoadFloat2(&box.MaxPoint) - XMLoadFloat2(&box.MinPoint))));
	}

	QuadTreeNode(QuadTreeNode* parent, XMFLOAT2 center, float size, int depth)
	{
		this->parent = parent;
		this->size = size;
		this->centerPosition = center;
		this->depth = depth;

		XMFLOAT2 minPoint, maxPoint;
		XMStoreFloat2(&minPoint, (XMLoadFloat2(&center) - (XMVectorSplatOne() * size * 0.5f)));
		XMStoreFloat2(&maxPoint, (XMLoadFloat2(&center) + (XMVectorSplatOne() * size * 0.5f)));

		boundingBox = new Lemmi2DAABB(minPoint, maxPoint);
	}

	~QuadTreeNode()
	{
		delete boundingBox;
		parent = 0;
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
						XMFLOAT2 subCenter = XMFLOAT2(centerPosition.x + (0.25f * size * (float)i), centerPosition.x + (0.25f * size * (float)j));
						float subSize = size * 0.5f;

						QuadTreeNode newNode = QuadTreeNode(&this, subCenter, subSize, depth + 1);

						children.push_back(newNode);
				}
			}
		}
	}

	///// <summary>
	///// Propagates objects down the tree up to maxDepth
	///// </summary>
	//void Propagate()
	//{
	//	if(children.size() > maxItems && depth <= maxDepth)
	//	{
	//		CreateSubNodes();

	//		for(int i = content.size()-1; i >= 0; i--)
	//		{
	//			//switch(OctreeContainmentMode)
	//			//{
	//			//case OctreeContainmentModes.Intersects:
	//			//	T item = Content[index];
	//			//	Content.Remove(Content[index]);

	//			//	for(int subNodeIndex = 0; subNodeIndex < SubNodes.Count; subNodeIndex++)
	//			//	{
	//			//		if(item.Intersect(SubNodes[subNodeIndex].BoundingBox))
	//			//		{
	//			//			SubNodes[subNodeIndex].Content.Add(item);
	//			//		}
	//			//	}
	//			//	break;

	//			//case OctreeContainmentModes.Contains:
	//				for(int j = 0; j < children.size(); j++)
	//				{
	//					/*************************************************************************************
	//					//If content[i] is contained by a smaller bounding box (of a sub node), 
	//					//then remove it from this list and add it to the subnode[j] content vector instead
	//					**************************************************************************************/

	//					//if(Content[i].IsContained(SubNodes[j].BoundingBox) == ContainmentType.Contains)
	//					//{
	//					//	SubNodes[j].Content.Add(Content[i]);
	//					//	Content.Remove(Content[i]);
	//					//	break;
	//					//}
	//				}
	//			}
	//		

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

/// <summary>
/// Interface to define objects that can be tested againt bounding primitives
/// </summary>
interface IOctreeable
{
	public:
	Lemmi2DAABB boundingBox;
	XMFLOAT2 position;
	int lastQueryID;
};