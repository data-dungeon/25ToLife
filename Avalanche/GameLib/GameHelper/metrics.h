/***************************************************************************/
// Basic class for metrics so we can manage them in a libarary
/***************************************************************************/
#ifndef METRICS_H
#define METRICS_H

#include "container/dllist.h"

class Metrics
{
public:
	Metrics(void) {}
	virtual ~Metrics(void) {}

	// For the list
	Metrics 	*prev, *next;
};

class MetricsLibrary
{
public:
	// A list of the metrics
	MetricsLibrary(void) : list(true) {}
	~MetricsLibrary(void) {}

	// Add a metircs to the libary
	void Add(Metrics *metrics)
		{ list.AddTail(metrics); }

private:
	// The list
	DoublyLinkedList<Metrics>	list;
};

#endif
