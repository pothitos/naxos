/**
   @file validator.cc 
   @brief Validates a solution for an instance on the Curriculum Based Course
   Timetabling track for the 2nd Intenational Timetabling Competition

   This file contains all class and function definitions of the validator.
   It is compiled on Linux (with GNU C++) with the command:
      g++ -o validator validator.cc
   It is used with the command (eg on instance 3):
      ./validator comp03.ctt my_sol.out

   The input file is assumed to be correct. The output file can be
   incorrect. An error message is produced if the output file is not
   correct.
    
   @author Andrea Schaerf (schaerf@uniud.it), 
           Luca Di Gaspero (l.digaspero@uniud.it)
   @version 1.1
   @date 25 October 2007
*/

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// BEGIN OF HEADER

class Course
{
 friend std::istream& operator>>(std::istream&, Course&);
public:
  const std::string& Name() const { return name; }
  const std::string& Teacher() const { return teacher; }
  unsigned Students() const { return students; }
  unsigned Lectures() const { return lectures; }
  unsigned MinWorkingDays() const { return min_working_days; }
private:
  std::string name, teacher;
  unsigned lectures, students, min_working_days;
};

class Curriculum 
{  
public:
  const std::string& Name() const { return name; }
  void SetName(const std::string& n) { name = n; }
  unsigned Size() const { return members.size(); }
  void AddMember(unsigned e) { members.push_back(e); }
  unsigned operator[](unsigned i) const { return members[i]; }
private:
  std::string name;
  std::vector<unsigned> members;
};

class Room
{
  friend std::istream& operator>>(std::istream&, Room&);
public:
  const std::string& Name() const { return name; }
  unsigned Capacity() const { return capacity; }
private:
  std::string name;
  unsigned capacity;
};

class Faculty
{
public:
  Faculty(const std::string& instance);

  unsigned Courses() const { return courses; }
  unsigned Rooms() const { return rooms; }
  unsigned Curricula() const { return curricula; }
  unsigned Periods() const { return periods; }
  unsigned PeriodsPerDay() const { return periods_per_day; }
  unsigned Days() const { return periods/periods_per_day; }

  bool  Available(unsigned c, unsigned p) const { return availability[c][p]; } 
  bool Conflict(unsigned c1, unsigned c2) const { return conflict[c1][c2]; } 
  const Course& CourseVector(int i) const { return course_vect[i]; }
  const Room& RoomVector(int i) const { return room_vect[i]; }
  const Curriculum& CurriculaVector(int i) const { return curricula_vect[i]; }

  bool CurriculumMember(unsigned c, unsigned g) const;

  int RoomIndex(const std::string&) const; 
  int CourseIndex(const std::string&) const;
  int CurriculumIndex(const std::string&) const;
  int PeriodIndex(const std::string&) const;
  const std::string& Name() const { return name; }

  const unsigned MIN_WORKING_DAYS_COST;
  const unsigned CURRICULUM_COMPACTNESS_COST;
  const unsigned ROOM_STABILITY_COST;

private:
  // scalar data
  std::string name; 
  unsigned rooms, courses, periods, periods_per_day, curricula;

  // data objects
  std::vector<Course> course_vect;
  std::vector<Room> room_vect;
  std::vector<Curriculum> curricula_vect;

  // availability and conflicts constraints
  std::vector<std::vector<bool> > availability;
  std::vector<std::vector<bool> > conflict;
};

class Timetable
{  
public:
  Timetable(const Faculty & f, const std::string file_name); 
  // Inspect timetable
  unsigned operator()(unsigned i, unsigned j) const { return tt[i][j]; }
  unsigned& operator()(unsigned i, unsigned j) { return tt[i][j]; }
  // Inspect redundant data  
  unsigned RoomLectures(unsigned i, unsigned j) const { return room_lectures[i][j]; }
  unsigned CurriculumPeriodLectures(unsigned i, unsigned j) const { return curriculum_period_lectures[i][j]; }
  unsigned CourseDailyLectures(unsigned i, unsigned j) const { return course_daily_lectures[i][j]; }
  unsigned WorkingDays(unsigned i) const { return working_days[i]; }
  unsigned UsedRoomsNo(unsigned i) const { return used_rooms[i].size(); }
  unsigned UsedRooms(unsigned i, unsigned j) const { return used_rooms[i][j]; }
  void InsertUsedRoom(unsigned i, unsigned j) { used_rooms[i].push_back(j); }
  unsigned Warnings() const { return warnings; }
  void UpdateRedundantData(); 

 private:
  const Faculty & in;  
  unsigned warnings;
  std::vector<std::vector<unsigned> > tt;  // (courses X periods) timetable matrix
    // redundant data
  std::vector<std::vector<unsigned> > room_lectures; // number of lectures per room in the same period (should be 0 or 1)
  std::vector<std::vector<unsigned> > curriculum_period_lectures; // number of lectures per curriculum in the same period (should be 0 or 1)
  std::vector<std::vector<unsigned> > course_daily_lectures; // number of lectures per course per day
  std::vector<unsigned> working_days; // number of days of lecture per course
  std::vector<std::vector<unsigned> > used_rooms; // rooms used for each lecture on the course
};



class Validator
{
public:
  Validator(const Faculty & i, const Timetable & o)
    : in(i), out(o) {}
  void PrintCosts(std::ostream& os) const;
  void PrintTotalCost(std::ostream& os) const;
  void PrintViolations(std::ostream& os) const;
private:
  unsigned CostsOnLectures() const;	       
  unsigned CostsOnConflicts() const;	       
  unsigned CostsOnAvailability() const;	       
  unsigned CostsOnRoomOccupation() const;     
  unsigned CostsOnRoomCapacity() const;	       
  unsigned CostsOnMinWorkingDays() const;        
  unsigned CostsOnCurriculumCompactness() const;
  unsigned CostsOnRoomStability() const;

  void PrintViolationsOnLectures(std::ostream& os) const;
  void PrintViolationsOnConflicts(std::ostream& os) const;
  void PrintViolationsOnAvailability(std::ostream& os) const;
  void PrintViolationsOnRoomOccupation(std::ostream& os) const;
  void PrintViolationsOnRoomCapacity(std::ostream& os) const;
  void PrintViolationsOnMinWorkingDays(std::ostream& os) const;
  void PrintViolationsOnCurriculumCompactness(std::ostream& os) const;
  void PrintViolationsOnRoomStability(std::ostream& os) const;

  const Faculty& in;  
  const Timetable& out; 
};

// END OF HEADER

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      std::cerr << "Usage:  " << argv[0] << "<input_file> <solution_file> " << std::endl;
      exit(1);
    }

  Faculty input(argv[1]);
  Timetable output(input, argv[2]);
  Validator validator(input, output);

  validator.PrintViolations(std::cout);
  std::cout << std::endl;
  validator.PrintCosts(std::cout);
  std::cout << std::endl;
  if (output.Warnings() > 0)
    std::cout << "There are " << output.Warnings() << " warnings!" << std::endl;
  std::cout << "Summary: ";
  validator.PrintTotalCost(std::cout);

  return 0;
}

std::istream& operator>>(std::istream& is, Course& c)
{
  return is >> c.name >> c.teacher >> c.lectures >> c.min_working_days >> c.students;
}

std::istream& operator>>(std::istream& is, Room& r)
{
  return is >> r.name >> r.capacity;
}

Timetable::Timetable(const Faculty &f, std::string file_name)
  : in(f),   tt(in.Courses(), std::vector<unsigned>(in.Periods(),0)),
    room_lectures(in.Rooms() + 1, std::vector<unsigned>(in.Periods())), 
    curriculum_period_lectures(in.Curricula(), std::vector<unsigned>(in.Periods())),
    course_daily_lectures(in.Courses(), std::vector<unsigned>(in.Days())), 
    working_days(in.Courses()), used_rooms(in.Courses())
{
  unsigned day, period, p;
  int c, r;
  std::string course_name, room_name;
  std::ifstream is(file_name.c_str());

  warnings = 0;

  if (is.fail())
    {
      std::cerr << "Output file does not exist!" << std::endl;
      exit(1);
    }

  for (unsigned i = 0; i < tt.size(); i++)
    for (unsigned j = 0; j < tt[i].size(); j++)
      tt[i][j] = 0;

  while (is >> course_name >> room_name >> day >> period)
    {
      c = in.CourseIndex(course_name);
      if (c == -1)
	{
	  std::cerr << "WARNING: Nonexisting course " << course_name << " (entry skipped)" << std::endl;
	  warnings++;
	  continue;
	}
      r = in.RoomIndex(room_name);
      if (r == -1)
	{
	  std::cerr << "WARNING: Nonexisting room " << room_name << " (entry skipped)" << std::endl;
	  warnings++;
	  continue;
	}

      if (day >= in.Days())
	{
	  std::cerr << "WARNING: Nonexisting day " << day << " (entry skipped)" << std::endl;
	  warnings++;
	  continue;
	}
      if (period >= in.PeriodsPerDay())
	{
	  std::cerr << "WARNING: Nonexisting period " << period << " (entry skipped)" << std::endl;
	  warnings++;
	  continue;
	}
      p = day * in.PeriodsPerDay() + period;
      if (tt[c][p] != 0)
	{
	  std::cerr << "WARNING: Repeated entry: " << course_name << " " << room_name << " " << day << " "  << period << " (entry skipped)" << std::endl;
	  warnings++;
	  continue;
	}
      tt[c][p] = r;
    }
  UpdateRedundantData();
}

void Timetable::UpdateRedundantData() 
{
  unsigned p, c, r, d, g, gn, i;
  
  for (r = 1; r < in.Rooms() + 1; r++)
    for (p = 0; p < in.Periods(); p++)
      room_lectures[r][p] = 0;

  gn = in.Curricula();
  for (g = 0; g < gn; g++)
    for (p = 0; p < in.Periods(); p++)
      curriculum_period_lectures[g][p] = 0;

  for (c = 0; c < in.Courses(); c++)
    {
      for (p = 0; p < in.Periods(); p++)
	{
	  r = tt[c][p];
	  if (r != 0)
	    room_lectures[r][p]++;
	}
    }

  for (c = 0; c < in.Courses(); c++)
    {
      for (g = 0; g < in.Curricula(); g++)
	if (in.CurriculumMember(c,g))
	  for (p = 0; p < in.Periods(); p++)
	    if (tt[c][p] != 0)
	      {
		curriculum_period_lectures[g][p]++;
	      }
    }

  for (c = 0; c < in.Courses(); c++)
    {
      working_days[c] = 0;
      for (d = 0; d < in.Days(); d++)
      {
	course_daily_lectures[c][d] = 0;
	for (p = d * in.PeriodsPerDay(); 
	     p < (d+1) * in.PeriodsPerDay(); p++)
	  {
	    if (tt[c][p] != 0)
	      course_daily_lectures[c][d]++; 
	  }
	if (course_daily_lectures[c][d] >= 1)
	  working_days[c]++;
      }
    }

  for (c = 0; c < in.Courses(); c++)
    {
      for (p = 0; p < in.Periods(); p++)
	{
	  r = tt[c][p];
	  if (r != 0)
	    {
	      for (i = 0; i < used_rooms[c].size(); i++)
		if (used_rooms[c][i] == r)
		  break;
	      if (i == used_rooms[c].size())
		{
		  used_rooms[c].push_back(r);
		}
	    }
	}
    }
} 

Faculty::Faculty(const std::string& file_name)
  : MIN_WORKING_DAYS_COST(5), CURRICULUM_COMPACTNESS_COST(2), ROOM_STABILITY_COST(1)
{ 
  std::string curriculum, course_name, room_name, period_name, 
    teacher_name, priority, buffer;
  std::string course_name1, course_name2;
  unsigned curriculum_size, days, constraints, i;
  std::ifstream is(file_name.c_str());

  if (is.fail())
    {
      std::cerr << "Input file does not exist!" << std::endl;
      exit(1);
    }
  
  is >> buffer >> name; 
  is >> buffer >> courses;
  is >> buffer >> rooms;
  is >> buffer >> days;
  is >> buffer >> periods_per_day;
  is >> buffer >> curricula;
  is >> buffer >> constraints;

  periods = days * periods_per_day;

  course_vect.resize(courses);
     // location 0 of room_vect is not used (teaching in room 0 means NOT TEACHING)
  room_vect.resize(rooms + 1);
  curricula_vect.resize(curricula);
  availability.resize(courses, std::vector<bool>(periods,true));
  conflict.resize(courses, std::vector<bool>(courses));

  is >> buffer;
  for (i = 0; i < courses; i++)
    is >> course_vect[i];

  is >> buffer;
  for (i = 1; i <= rooms; i++)
    is >> room_vect[i];

  is >> buffer;
  for (i = 0; i < curricula; i++)
    {
      is >> buffer >> curriculum_size;
      curricula_vect[i].SetName(buffer);
      unsigned i1, i2;
      for (i1 = 0; i1 < curriculum_size; i1++)
	{
	  int c1; 
	  unsigned c2;
	  is >> course_name;
	  c1 = CourseIndex(course_name);
	  curricula_vect[i].AddMember(c1);
	  for (i2 = 0; i2 < i1; i2++)
	    {
	      c2 = curricula_vect[i][i2];
	      conflict[c1][c2] = true;
	      conflict[c2][c1] = true;
	    }
	}
    }

  is >> buffer;
  int c, p, period_index, day_index;
  
   for (i = 0; i < constraints; i++)
     {
       is >> course_name  >> day_index >> period_index;
       p = day_index * periods_per_day +  period_index;
       c = CourseIndex(course_name);
       availability[c][p] = false;
     }

  // Add same-teacher constraints
  for (unsigned c1 = 0; c1 < courses - 1; c1++)
    for (unsigned c2 = c1+1; c2 < courses; c2++)
      if (course_vect[c1].Teacher() == course_vect[c2].Teacher())
	{
	  conflict[c1][c2] = true;
	  conflict[c2][c1] = true;
	}
}


int Faculty::CourseIndex(const std::string& name) const
{
  for (unsigned i = 0; i < course_vect.size(); i++)
    if (course_vect[i].Name() == name)
      return i;
  return -1;
}

int Faculty::CurriculumIndex(const std::string& name) const
{
  for (unsigned i = 0; i < curricula_vect.size(); i++)
    if (curricula_vect[i].Name() == name)
      return i;
  return -1;    
}

int Faculty::RoomIndex(const std::string& name) const
{
  for (unsigned i = 0; i < room_vect.size(); i++)
    if (room_vect[i].Name() == name)
      return i;
  return -1;
}

bool Faculty::CurriculumMember(unsigned c, unsigned g) const
{  
  for (unsigned i = 0; i < curricula_vect[g].Size(); i++)
    if (curricula_vect[g][i] == c)
      return true;
  return false;
}



void Validator::PrintTotalCost(std::ostream& os)  const
{
  unsigned violations = CostsOnLectures() + CostsOnConflicts() + CostsOnAvailability() + CostsOnRoomOccupation();

  if (violations > 0)
    os << "Violations = " << violations << ", ";
  os <<  "Total Cost = " << CostsOnRoomCapacity() + CostsOnMinWorkingDays() * in.MIN_WORKING_DAYS_COST
    + CostsOnCurriculumCompactness() * in.CURRICULUM_COMPACTNESS_COST + CostsOnRoomStability() * in.ROOM_STABILITY_COST << std::endl;
}

void Validator::PrintCosts(std::ostream& os) const
{
  os << "Violations of Lectures (hard) : " <<  CostsOnLectures() << std::endl;
  os << "Violations of Conflicts (hard) : " <<  CostsOnConflicts() << std::endl;
  os << "Violations of Availability (hard) : " <<  CostsOnAvailability() << std::endl;
  os << "Violations of RoomOccupation (hard) : " <<  CostsOnRoomOccupation() << std::endl;
  os << "Cost of RoomCapacity (soft) : " <<  CostsOnRoomCapacity() << std::endl;
  os << "Cost of MinWorkingDays (soft) : " <<  CostsOnMinWorkingDays() * in.MIN_WORKING_DAYS_COST << std::endl;
  os << "Cost of CurriculumCompactness (soft) : " <<  CostsOnCurriculumCompactness() * in.CURRICULUM_COMPACTNESS_COST << std::endl;
  os << "Cost of RoomStability (soft) : " <<  CostsOnRoomStability() * in.ROOM_STABILITY_COST << std::endl;
}

unsigned Validator::CostsOnLectures() const
{
  unsigned c, p, cost = 0, lectures;
  for (c = 0; c < in.Courses(); c++)
    {
      lectures = 0;
      for (p = 0; p < in.Periods(); p++)
	if (out(c,p) != 0)
	  lectures++;
      if (lectures < in.CourseVector(c).Lectures())
	cost += (in.CourseVector(c).Lectures() - lectures);
      else if (lectures > in.CourseVector(c).Lectures())
	cost += (lectures - in.CourseVector(c).Lectures());
    }
  return cost;
}

unsigned Validator::CostsOnConflicts() const
{
  unsigned c1, c2, p, cost = 0;
  for (c1 = 0; c1 < in.Courses(); c1++)
    for (c2 = c1+1; c2 < in.Courses(); c2++)
      if (in.Conflict(c1,c2))
	{
	  for (p = 0; p < in.Periods(); p++)
	    if (out(c1,p) != 0 && out(c2,p) != 0)
	      cost++;
	}
  return cost;

}
	       
unsigned Validator::CostsOnAvailability() const	       
{
  unsigned c, p, cost = 0;
  for (c = 0; c < in.Courses(); c++)
    for (p = 0; p < in.Periods(); p++)
      if (out(c,p) != 0 && !in.Available(c,p))
	cost++;
  return cost;
}

unsigned Validator::CostsOnRoomOccupation() const
{
  unsigned r, p, cost = 0;
  for (p = 0; p < in.Periods(); p++)
    for (r = 1; r <= in.Rooms(); r++)
      if (out.RoomLectures(r,p) > 1)
	cost+= out.RoomLectures(r,p) - 1; 
 return cost;

}
     
unsigned Validator::CostsOnRoomCapacity() const
{
  unsigned c, p, r, cost = 0;
  for (c = 0; c < in.Courses(); c++)
    for (p = 0; p < in.Periods(); p++)
      {
	r = out(c,p);
	if (r != 0 && in.RoomVector(r).Capacity() < in.CourseVector(c).Students())
	  cost += in.CourseVector(c).Students() - in.RoomVector(r).Capacity();
      }
  return cost;
}
	       
unsigned Validator::CostsOnMinWorkingDays() const
{
  unsigned c, cost = 0;
  for (c = 0; c < in.Courses(); c++)
    if (out.WorkingDays(c) < in.CourseVector(c).MinWorkingDays())
      cost += in.CourseVector(c).MinWorkingDays() - out.WorkingDays(c);
  return cost;

}
        
unsigned Validator::CostsOnCurriculumCompactness() const
{
  unsigned g, p, cost = 0, ppd = in.PeriodsPerDay();

  for (g = 0; g < in.Curricula(); g++)
    {
      for (p = 0; p < in.Periods(); p++)
	if (out.CurriculumPeriodLectures(g,p) > 0)
	  {
	    if (p % ppd == 0 && out.CurriculumPeriodLectures(g,p+1) == 0)
	      cost += out.CurriculumPeriodLectures(g,p);
	    else if (p % ppd == ppd-1 && out.CurriculumPeriodLectures(g,p-1) == 0)
	      cost += out.CurriculumPeriodLectures(g,p);
	    else if (out.CurriculumPeriodLectures(g,p+1) == 0 && out.CurriculumPeriodLectures(g,p-1) == 0)
	      cost += out.CurriculumPeriodLectures(g,p);
	  }
    }
  return cost;
}

unsigned Validator::CostsOnRoomStability() const
{
  unsigned c, cost = 0;

  for (c = 0; c < in.Courses(); c++)
    if (out.UsedRoomsNo(c) > 1)
	cost += out.UsedRoomsNo(c) - 1;
  return cost;
}



void Validator::PrintViolations(std::ostream& os) const
{
  PrintViolationsOnLectures(os);
  PrintViolationsOnConflicts(os);
  PrintViolationsOnAvailability(os);
  PrintViolationsOnRoomOccupation(os);
  PrintViolationsOnRoomCapacity(os);
  PrintViolationsOnMinWorkingDays(os);
  PrintViolationsOnCurriculumCompactness(os);
  PrintViolationsOnRoomStability(os);
}

void Validator::PrintViolationsOnLectures(std::ostream& os) const
{
  unsigned c, p, lectures;
  for (c = 0; c < in.Courses(); c++)
    {
      lectures = 0;
      for (p = 0; p < in.Periods(); p++)
	if (out(c,p) != 0)
	  lectures++;
      if (lectures < in.CourseVector(c).Lectures())
	os << "[H] Too few lectures for course " << in.CourseVector(c).Name() << std::endl;
      else if (lectures > in.CourseVector(c).Lectures())
	os << "[H] Too many lectures for course " << in.CourseVector(c).Name() << std::endl;
    }
}




void Validator::PrintViolationsOnConflicts(std::ostream& os) const
{
  unsigned c1, c2, p;
  for (c1 = 0; c1 < in.Courses(); c1++)
    for (c2 = c1+1; c2 < in.Courses(); c2++)
      if (in.Conflict(c1,c2))
	{
	  for (p = 0; p < in.Periods(); p++)
	    if (out(c1,p) != 0 && out(c2,p) != 0)
	      os << "[H] Courses " << in.CourseVector(c1).Name() << " and " 
		 << in.CourseVector(c2).Name() << " have both a lecture at period " 
		 << p << " (day " << p/in.PeriodsPerDay() << ", timeslot " << p % in.PeriodsPerDay() << ")" << std::endl;
	}
}

void Validator::PrintViolationsOnAvailability(std::ostream& os) const
{
  unsigned c, p;
  for (c = 0; c < in.Courses(); c++)
    for (p = 0; p < in.Periods(); p++)
      if (out(c,p) != 0 && !in.Available(c,p))
	os << "[H] Course " << in.CourseVector(c).Name() << " has a lecture at unavailable period "
	   << p << " (day " << p/in.PeriodsPerDay() << ", timeslot " << p % in.PeriodsPerDay() << ")" << std::endl;
}

void Validator::PrintViolationsOnRoomOccupation(std::ostream& os) const
{
  unsigned r, p;
  for (p = 0; p < in.Periods(); p++)
    for (r = 1; r <= in.Rooms(); r++)
      if (out.RoomLectures(r,p) > 1)
	{
	  os << "[H] " << out.RoomLectures(r,p) << " lectures in room " 
	     << in.RoomVector(r).Name() << " the period " << p << " (day " << p/in.PeriodsPerDay() << ", timeslot " << p % in.PeriodsPerDay() << ")"; 
	  if (out.RoomLectures(r,p) > 2)
	    os << " [" << out.RoomLectures(r,p) - 1 << " violations]";
	  os << std::endl;
	}
}

void Validator::PrintViolationsOnRoomCapacity(std::ostream& os) const
{
 unsigned c, p, r;
  for (c = 0; c < in.Courses(); c++)
    for (p = 0; p < in.Periods(); p++)
      {
	r = out(c,p);
	if (r != 0 && in.RoomVector(r).Capacity() < in.CourseVector(c).Students())
	  os << "[S(" << in.CourseVector(c).Students() - in.RoomVector(r).Capacity() 
	     << ")] Room " << in.RoomVector(r).Name() << " too small for course " 
	     << in.CourseVector(c).Name() << " the period " 
	     << p << " (day " << p/in.PeriodsPerDay() << ", timeslot " << p % in.PeriodsPerDay() << ")" 
	     << std::endl;
      }
}

void Validator::PrintViolationsOnMinWorkingDays(std::ostream& os) const
{
  unsigned c;
  for (c = 0; c < in.Courses(); c++)
    if (out.WorkingDays(c) < in.CourseVector(c).MinWorkingDays())
      os << "[S(" << in.MIN_WORKING_DAYS_COST << ")] The course " << in.CourseVector(c).Name() << " has only " << out.WorkingDays(c)
         << " days of lecture" << std::endl;
}

void Validator::PrintViolationsOnCurriculumCompactness(std::ostream& os) const
{
   unsigned g, p, ppd = in.PeriodsPerDay();

  for (g = 0; g < in.Curricula(); g++)
    {
      for (p = 0; p < in.Periods(); p++)
	if (out.CurriculumPeriodLectures(g,p) > 0)
	  {
	    if ((p % ppd == 0 && out.CurriculumPeriodLectures(g,p+1) == 0)
		|| (p % ppd == ppd-1 && out.CurriculumPeriodLectures(g,p-1) == 0)
		|| (out.CurriculumPeriodLectures(g,p+1) == 0 && out.CurriculumPeriodLectures(g,p-1) == 0))
	      os << "[S(" << in.CURRICULUM_COMPACTNESS_COST << ")] Curriculum " << in.CurriculaVector(g).Name() 
		 << " has an isolated lecture at period " << p << " (day " << p/in.PeriodsPerDay() << ", timeslot " << p % in.PeriodsPerDay() << ")"
		 << std::endl;
	  }
    }
}

void Validator::PrintViolationsOnRoomStability(std::ostream& os) const
{
  std::vector<unsigned> used_rooms;
  unsigned c;

  for (c = 0; c < in.Courses(); c++)
    if (out.UsedRoomsNo(c) > 1)
      os << "[S(" << (out.UsedRoomsNo(c) - 1) * in.ROOM_STABILITY_COST << ")] Course " << in.CourseVector(c).Name() << " uses " 
	 << out.UsedRoomsNo(c) << " different rooms" << std::endl;
}
