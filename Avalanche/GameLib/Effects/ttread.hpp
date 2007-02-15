/***************************************************************************/
// implementations for the tire track group
/***************************************************************************/

/***************************************************************************/
// Return the oldest deletable track in the list
// Note, list is in in age order
/***************************************************************************/
inline TireTrack *TireTreadType::Oldest(void)
{
	TireTrack *track = list.Head();
	while (track)
	{
		// IF the track is locked, or it was drawn the last frame, skip
		if (track->Locked() || track->DrawnLastFrame())
			track = track->next;
		else
			return(track);
	}

	return(track);
}

/***************************************************************************/
// Culling
/***************************************************************************/
inline bool TireTreadType::CullTireTrack(
Graphics4x4 &camera,
TireTrack &track,
float maxDrawDistance)
{
	// Enough verts to draw?
	if (track.VertPairs() < 2)
		return(true);

	Vector3Packed *v = track.GetMiddleVert();
	Vector3 vt = *v * camera;

	// in front?
	if (vt.z() < -maxDrawDistance || vt.z() > maxDrawDistance)
		return(true);

	// passed all the tests
	return(false);
}






