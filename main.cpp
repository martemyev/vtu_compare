#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void get_data(const std::string &filename,
              std::vector<std::vector<double> > &data);
void write_difference(const std::string &filename,
                      const std::string &diff_file,
                      const std::vector<std::vector<double> > &diff);




int main(int argc, char **argv)
{
  if (argc < 5)
  {
    std::cout << "\nUsage: ./prog file_0.vtu file_1.vtu diff_file.vtu K1 K2 ... KN\n\n";
    std::cout << "file_0      - name of one file for comparison\n";
    std::cout << "file_1      - name of another file for comparison\n";
    std::cout << "diff_file   - name of file with difference\n";
    std::cout << "K1          - size of first distinct solution (total number of dofs)\n";
    std::cout << "K2          - size of second distinct solution (total number of dofs)\n";
    std::cout << "KN          - size of N-th distinct solution (total number of dofs)\n\n";
    return 1;
  }

  const std::string file_0    = std::string(argv[1]);
  const std::string file_1    = std::string(argv[2]);
  const std::string diff_file = std::string(argv[3]);

  int n_datasets = argc - 4; // number of distinct solutions

  std::vector<std::vector<double> > data_0(n_datasets); // from file_0
  std::vector<std::vector<double> > data_1(n_datasets); // from file_1
  std::vector<std::vector<double> > diff(n_datasets); // difference

  std::cout << "file_0    = " << file_0 << "\n";
  std::cout << "file_1    = " << file_1 << "\n";
  std::cout << "diff_file = " << diff_file << "\n";
  for (size_t i = 0; i < n_datasets; ++i)
  {
    int K = atoi(argv[i + 4]);
    std::cout << "K" << i+1 << "        = " << K << "\n";
    data_0[i].resize(K);
    data_1[i].resize(K);
    diff[i].resize(K);
  }

  get_data(file_0, data_0);
  get_data(file_1, data_1);

  for (size_t d = 0; d < data_0.size(); ++d)
  {
    double L2_0 = 0, L2_1 = 0, L2_diff = 0;
    for (size_t i = 0; i < data_0[d].size(); ++i)
    {
      const double d0 = data_0[d][i];
      const double d1 = data_1[d][i];
      L2_0 += d0 * d0;
      L2_1 += d1 * d1;
      diff[d][i] = d0 - d1;
      L2_diff += diff[d][i] * diff[d][i];
    }
    L2_0 = sqrt(L2_0);
    L2_1 = sqrt(L2_1);
    L2_diff = sqrt(L2_diff);

    std::cout << "dataset " << d+1 << std::endl;
    std::cout << "L2_0 = " << L2_0 << std::endl;
    std::cout << "L2_1 = " << L2_1 << std::endl;
    std::cout << "L2_diff = " << L2_diff << " = " << L2_diff * 100 << " %"
              << std::endl;
  }

  write_difference(file_0, diff_file, diff);

  return 0;
}





void get_data(const std::string &filename,
              std::vector<std::vector<double> > &data)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    std::cerr << "File " << filename << " can't be opened" << std::endl;
    throw 1;
  }

  std::string tmp; // temporary lines

  for (int i = 0; i < 6; ++i)
    getline(in, tmp);

  for (size_t d = 0; d < data.size(); ++d)
  {
    for (size_t i = 0; i < data[d].size(); ++i)
      in >> data[d][i];
    for (int i = 0; i < 2; ++i)
      getline(in, tmp);
  }
}




void write_difference(const std::string &filename,
                      const std::string &diff_file,
                      const std::vector<std::vector<double> > &diff)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    std::cerr << "File " << filename << " can't be opened for reading\n";
    exit(1);
  }

  std::ofstream out(diff_file.c_str());
  if (!out)
  {
    {
      std::cerr << "File " << diff_file << " can't be opened for writing\n";
      exit(1);
    }
  }

  std::string line;

  for (int i = 0; i < 6; ++i)
  {
    getline(in, line);
    out << line << "\n";
  }

  double tmp;
  for (size_t d = 0; d < diff.size(); ++d)
  {
    for (size_t i = 0; i < diff[d].size(); ++i)
    {
      in >> tmp;
      out << diff[d][i] << " ";
    }
    for (int i = 0; i < 2; ++i)
    {
      getline(in, line);
      out << line << "\n";
    }
  }

  while (getline(in, line))
    out << line << "\n";
}

