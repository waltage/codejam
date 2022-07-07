package templates

import (
	_ "embed"
	"fmt"
	"log"
	"os"
	"path"
	"text/template"
)

//go:embed "templ/clion_cmake_config_template.xml"
var TemplClionCmakeProblemConfigs string

//go:embed "templ/clion_config_template.xml"
var TemplClionCmakeConfig string

//go:embed "templ/cmake_cmakelists_template.txt"
var TemplCmakeLists string

//go:embed "templ/cpp_base_template.cpp"
var TemplCppProblem string

type ProjectConfig struct {
	Name         string
	ProblemCount int
	ProbArray    []string
}

func NewProject(name string, nProblems int) *ProjectConfig {
	conf := &ProjectConfig{
		Name:         name,
		ProblemCount: nProblems,
	}
	for p := 1; p < conf.ProblemCount+1; p++ {
		conf.ProbArray = append(conf.ProbArray, fmt.Sprintf("%02d", p))
	}
	return conf
}

func (c *ProjectConfig) BuildDirectories() {
	err := os.MkdirAll(
		path.Join(c.Name, ".idea", "runConfigurations"),
		os.ModePerm,
	)
	if err != nil {
		log.Fatalf("could not build directory: %s", err)
	}
}

func (c *ProjectConfig) WriteProblemFiles() {
	var err error
	var fileName string
	var f *os.File
	for p := 1; p < c.ProblemCount+1; p++ {
		// empty #.stdin.txt
		fileName = fmt.Sprintf("problem_%02d.stdin.txt", p)
		f, err = os.Create(path.Join(c.Name, fileName))

		if err != nil {
			log.Fatalf("could not create stdin file %02d: %s", p, err)
		}
		_ = f.Close()

		// fill #.cpp
		fileName = fmt.Sprintf("problem_%02d.cpp", p)
		f, err = os.OpenFile(path.Join(c.Name, fileName),
			os.O_CREATE|os.O_WRONLY, os.ModePerm)
		if err != nil {
			log.Fatalf("could not create cpp file %02d: %s", p, err)
		}
		_, err = f.WriteString(TemplCppProblem)
		if err != nil {
			log.Fatalf("could not write cpp template %02d: %s", p, err)
		}
		_ = f.Close()
	}
}

func (c *ProjectConfig) WriteClionConfigs() {
	var err error
	var fileName string
	var f *os.File

	fileName = path.Join(c.Name, ".idea", ".name")
	f, err = os.OpenFile(fileName, os.O_CREATE|os.O_WRONLY, os.ModePerm)
	if err != nil {
		log.Fatalf("could not create .name file: %s", err)
	}
	_, err = f.WriteString(c.Name)
	if err != nil {
		log.Fatalf("could not write .name file: %s", err)
	}

	fillTemplateToFile(
		"cmake lists",
		path.Join(c.Name, "CMakeLists.txt"),
		TemplCmakeLists,
		c,
	)

	fillTemplateToFile(
		"idea cmake config",
		path.Join(c.Name, ".idea", "cmake.xml"),
		TemplClionCmakeProblemConfigs,
		c,
	)

	fillTemplateToFile(
		"idea cmake run configs",
		path.Join(c.Name, ".idea", "cmake.xml"),
		TemplClionCmakeProblemConfigs,
		c,
	)

	t := template.Must(
		template.New("clion problem runconf").Parse(TemplClionCmakeConfig))
	for p := 1; p < c.ProblemCount+1; p++ {
		fileName = path.Join(c.Name, ".idea", "runConfigurations",
			fmt.Sprintf("problem_%02d.xml", p))
		f, err = os.OpenFile(fileName, os.O_CREATE|os.O_WRONLY, os.ModePerm)
		if err != nil {
			log.Fatalf("could not create runconf file %02d: %s", p, err)
		}
		err = t.Execute(f, map[string]interface{}{
			"ProblemID": fmt.Sprintf("%02d", p),
			"Name":      c.Name,
		})
		if err != nil {
			log.Fatalf("could not write to runconf file %02d: %s", p, err)
		}
		_ = f.Close()
	}
}

func fillTemplateToFile(name, path, templ string, conf *ProjectConfig) {
	t := template.Must(template.New(name).Parse(templ))
	f, err := os.OpenFile(path, os.O_CREATE|os.O_WRONLY, os.ModePerm)
	if err != nil {
		log.Fatalf("could not create file for template '%s': %s", name, err)
	}
	err = t.Execute(f, conf)
	if err != nil {
		log.Fatalf("could not parse template '%s': %s", name, err)
	}
	_ = f.Close()
}
