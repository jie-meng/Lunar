#include "base.hpp"
#include "csv.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <istream>
#include "file.hpp"

using namespace std;

namespace util
{

typedef UtilSharedPtr< vector<string> > SpStrVec;

struct Csv::CsvImpl
{
    CsvImpl(char delimeter, char enclosure) :
        delimeter_(delimeter),
        enclosure_(enclosure)
    {}

    CsvImpl(const std::string& file, char delimeter, char enclosure) :
        delimeter_(delimeter),
        enclosure_(enclosure)
    {
        read(file);
    }

    void clear()
    {
        file_name_ = "";
        matrix_.clear();
    }

    bool read(const std::string& file)
    {
        clear();

        string text = readTextFile(file);
        if (text == "")
            return false;

        std::stringstream in(text);

        std::stringstream ss;
        bool inquotes = false;

        SpStrVec spstrvec(new vector<string>());

        while(in.good())
        {
            char c = in.get();
            if (!inquotes && c==enclosure_) //beginquotechar
            {
                inquotes=true;
            }
            else if (inquotes && c==enclosure_) //quotechar
            {
                if ( in.peek() == enclosure_)//2 consecutive quotes resolve to 1
                {
                    ss << (char)in.get();
                }
                else //endquotechar
                {
                    inquotes=false;
                }
            }
            else if (!inquotes && c==delimeter_) //end of field
            {
                spstrvec->push_back( ss.str() );
                ss.str("");
            }
            else if (!inquotes && (c=='\r' || c=='\n') )
            {
                if(in.peek()=='\n') { in.get(); }
                spstrvec->push_back( ss.str() );
                ss.str("");

                if (!empty() && spstrvec->size() != cols())
                {
                    clear();
                    return false;
                }

                matrix_.push_back(spstrvec);
                spstrvec.reset(new vector<string>());
            }
            else
            {
                ss << c;
            }
        }

        file_name_ = file;
        return true;
    }

    bool write(const std::string& file)
    {
        std::ofstream ofs;
        ofs.open(file.c_str());
        if (ofs.is_open())
        {
            string delemiter = strFormat("%c", delimeter_);
            string enclosure = strFormat("%c", enclosure_);

            for (size_t i = 0; i < matrix_.size(); ++i)
            {
                std::string str_join = "";
                SpStrVec spstrvec = matrix_[i];
                vector<string>::iterator c_it;
                for (c_it = spstrvec->begin(); c_it != spstrvec->end(); ++c_it)
                {
                    string str = *c_it;
                    str = strReplaceAll(str, enclosure, enclosure+enclosure);

                    if (strContains(str, delemiter) ||
                        strContains(str, enclosure) ||
                        strContains(str, "\r") ||
                        strContains(str, "\n"))
                        str_join += enclosure + str + enclosure + delemiter;
                    else
                        str_join += *c_it + delemiter;
                }

                if (strEndWith(str_join, delemiter))
                    str_join = str_join.substr(0, str_join.length() - 1);

                ofs<<str_join + "\n";
            }

            ofs.close();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool write()
    {
        return (file_name_ == "") ? false : write(file_name_);
    }

    bool empty() const
    {
        return matrix_.empty();
    }

    size_t rows() const
    {
        return matrix_.size();
    }

    size_t cols() const
    {
        if (matrix_.empty())
            return 0;

        return matrix_[0]->size();
    }

    std::string getCellValue(size_t row, size_t col) const
    {
        if (row < rows() && col < cols())
            return matrix_[row]->at(col);
        else
            return "";
    }

    bool setCellValue(size_t row, size_t col, const std::string& value)
    {
        if (row < rows() && col < cols())
        {
            matrix_[row]->at(col) = value;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool addRow(const std::vector<std::string>& vec)
    {
        if (vec.empty())
            return false;

        if (empty())
        {
            matrix_.push_back(SpStrVec(new vector<string>(vec)));
            return true;
        }
        else
        {
            vector<string>* pvec = NULL;
            if (vec.size() < cols())
            {
                pvec = new vector<string>(cols());
                for (size_t i=0; i<cols(); ++i)
                {
                    if (i < vec.size())
                        (*pvec)[i] = vec[i];
                    else
                        (*pvec)[i] = "";
                }
            }
            else
            {
                pvec = new vector<string>(vec.begin(), vec.begin() + cols());
            }

            matrix_.push_back(SpStrVec(pvec));
            return true;
        }
    }

    std::string file_name_;
    vector<SpStrVec> matrix_;
    char delimeter_;
    char enclosure_;
};

Csv::Csv(char delimeter,
         char enclosure) :
    pimpl_(new CsvImpl(delimeter, enclosure))
{
}

Csv::Csv(const std::string& file,
         char delimeter,
         char enclosure) :
    pimpl_(new CsvImpl(file, delimeter, enclosure))
{}

Csv::~Csv()
{}

bool Csv::read(const std::string& file)
{
    return pimpl_->read(file);
}

bool Csv::write(const std::string& file)
{
    return pimpl_->write(file);
}

bool Csv::write()
{
    return pimpl_->write();
}

void Csv::clear()
{
    return pimpl_->clear();
}

bool Csv::empty() const
{
    return pimpl_->empty();
}

size_t Csv::rows() const
{
    return pimpl_->rows();
}

size_t Csv::cols() const
{
    return pimpl_->cols();
}

std::string Csv::getCellValue(size_t row, size_t col) const
{
    return pimpl_->getCellValue(row, col);
}

bool Csv::setCellValue(size_t row, size_t col, const std::string& value)
{
    return pimpl_->setCellValue(row, col, value);
}

bool Csv::addRow(const std::vector<std::string>& vec)
{
    return pimpl_->addRow(vec);
}

} //namespace util
